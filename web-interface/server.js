const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const cors = require('cors');
const axios = require('axios');
const cron = require('node-cron');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = socketIo(server, {
    cors: {
        origin: "*",
        methods: ["GET", "POST"]
    }
});

app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

// In-memory storage for connected boxes
const connectedBoxes = new Map();
const puzzleDefinitions = new Map();

/*
 Pink=0
 Red=1
 Orange=2
 Blue=3
 Violet=4
 Yellow=5
 White=6
 Black=7
 Brown=8
 Green=9*/ 
 
// Default puzzle definitions
const defaultPuzzles = [
    {
        id: 'puzzle1',
        name: 'Green Box 1',
        layout: [9, 7, 5, 
                9, 5, 9, 
                5, 7, 9], // ButtonType enum values
        solveColor: 9, // Green
        description: 'Get all corners green'
    },
    {
        id: 'puzzle2', 
        name: 'Red Box 1',
        layout: [2, 8, 2, 
                2, 1, 2, 
                2, 8, 2],
        solveColor: 1, // Red
        description: 'Turn all corners red'
    },
    {
        id: 'puzzle3', 
        name: 'Black Box 1',
        layout: [9, 8, 9, 
                8, 8, 8, 
                9, 5, 9],
        solveColor: 8, // Brown
        description: 'Turn all corners brown'
    }
];

// Initialize default puzzles
defaultPuzzles.forEach(puzzle => {
    puzzleDefinitions.set(puzzle.id, puzzle);
});

// Box discovery and status endpoints
app.post('/api/boxes/register', (req, res) => {
    const { boxId, ip } = req.body;
    connectedBoxes.set(boxId, {
        id: boxId,
        ip: ip,
        lastSeen: Date.now(),
        status: 'online',
        currentPuzzle: null
    });
    
    io.emit('boxConnected', { boxId, ip });
    res.json({ status: 'registered' });
});

app.get('/api/boxes', (req, res) => {
    const boxes = Array.from(connectedBoxes.values());
    res.json(boxes);
});

app.get('/api/boxes/:boxId/status', async (req, res) => {
    const { boxId } = req.params;
    const box = connectedBoxes.get(boxId);
    
    if (!box) {
        return res.status(404).json({ error: 'Box not found' });
    }
    
    try {
        const response = await axios.get(`http://${box.ip}/api/status`);
        res.json(response.data);
    } catch (error) {
        res.status(500).json({ error: 'Failed to get box status' });
    }
});

// Puzzle management endpoints
app.get('/api/puzzles', (req, res) => {
    const puzzles = Array.from(puzzleDefinitions.values());
    res.json(puzzles);
});

app.post('/api/puzzles', (req, res) => {
    const puzzle = req.body;
    puzzle.id = puzzle.id || `puzzle_${Date.now()}`;
    puzzleDefinitions.set(puzzle.id, puzzle);
    
    io.emit('puzzleAdded', puzzle);
    res.json(puzzle);
});

app.put('/api/puzzles/:puzzleId', (req, res) => {
    const { puzzleId } = req.params;
    const puzzle = req.body;
    puzzle.id = puzzleId;
    
    puzzleDefinitions.set(puzzleId, puzzle);
    io.emit('puzzleUpdated', puzzle);
    res.json(puzzle);
});

app.delete('/api/puzzles/:puzzleId', (req, res) => {
    const { puzzleId } = req.params;
    puzzleDefinitions.delete(puzzleId);
    
    io.emit('puzzleDeleted', { puzzleId });
    res.json({ status: 'deleted' });
});

// Deploy puzzle to box(es)
app.post('/api/deploy', async (req, res) => {
    const { puzzleId, boxIds } = req.body;
    const puzzle = puzzleDefinitions.get(puzzleId);
    
    if (!puzzle) {
        return res.status(404).json({ error: 'Puzzle not found' });
    }
    
    const results = [];
    
    for (const boxId of boxIds) {
        const box = connectedBoxes.get(boxId);
        if (!box) {
            results.push({ boxId, status: 'error', message: 'Box not found' });
            continue;
        }
        
        try {
            const formData = new URLSearchParams();
            formData.append('data', JSON.stringify({
                layout: puzzle.layout,
                solveColor: puzzle.solveColor
            }));
            
            await axios.post(`http://${box.ip}/api/puzzle`, formData, {
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded'
                }
            });
            
            box.currentPuzzle = puzzleId;
            results.push({ boxId, status: 'success' });
            
        } catch (error) {
            results.push({ boxId, status: 'error', message: error.message });
        }
    }
    
    io.emit('puzzleDeployed', { puzzleId, results });
    res.json({ results });
});

// Reset box(es)
app.post('/api/reset', async (req, res) => {
    const { boxIds } = req.body;
    const results = [];
    
    for (const boxId of boxIds) {
        const box = connectedBoxes.get(boxId);
        if (!box) {
            results.push({ boxId, status: 'error', message: 'Box not found' });
            continue;
        }
        
        try {
            await axios.post(`http://${box.ip}/api/reset`);
            results.push({ boxId, status: 'success' });
        } catch (error) {
            results.push({ boxId, status: 'error', message: error.message });
        }
    }
    
    res.json({ results });
});

// WebSocket connections
io.on('connection', (socket) => {
    console.log('Client connected:', socket.id);
    
    // Send current state to new client
    socket.emit('boxList', Array.from(connectedBoxes.values()));
    socket.emit('puzzleList', Array.from(puzzleDefinitions.values()));
    
    socket.on('disconnect', () => {
        console.log('Client disconnected:', socket.id);
    });
});

// Periodic box health check
cron.schedule('*/5 * * * * *', async () => {
    const now = Date.now();
    const boxesToCheck = Array.from(connectedBoxes.values());
    
    for (const box of boxesToCheck) {
        try {
            console.log(`Checking box ${box.id} at ${box.ip}`);
            const response = await axios.get(`http://${box.ip}/api/status`, { 
                timeout: 3000,
                headers: {
                    'User-Agent': 'PuzzleBoxManager/1.0'
                }
            });
            
            box.lastSeen = now;
            box.status = 'online';
            
            console.log(`Box ${box.id} status:`, response.data);
            
            // Broadcast live status updates
            io.emit('boxStatus', {
                boxId: box.id,
                status: response.data
            });
            
        } catch (error) {
            console.log(`Error checking box ${box.id}:`, error.message);
            if (now - box.lastSeen > 30000) { // 30 second timeout
                box.status = 'offline';
                io.emit('boxOffline', { boxId: box.id });
            }
        }
    }
});

const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
    console.log(`Puzzle Box Manager server running on port ${PORT}`);
    console.log(`Web interface: http://localhost:${PORT}`);
});