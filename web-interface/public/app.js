// Socket.IO connection
const socket = io();

// Global state
let connectedBoxes = new Map();
let availablePuzzles = new Map();
let selectedPuzzleForDeploy = null;

// Color mapping
const buttonColors = [
    'pink', 'red', 'orange', 'blue', 'violet',
    'yellow', 'white', 'black', 'brown', 'green'
];

const buttonNames = [
    'Pink', 'Red', 'Orange', 'Blue', 'Violet',
    'Yellow', 'White', 'Black', 'Brown', 'Green'
];

// Initialize app
document.addEventListener('DOMContentLoaded', function() {
    initializeApp();
    createPuzzleGrid();
});

function initializeApp() {
    // Load initial data
    loadBoxes();
    loadPuzzles();
    
    // Set up socket listeners
    setupSocketListeners();
}

function setupSocketListeners() {
    socket.on('connect', () => {
        document.getElementById('connectionStatus').textContent = 'Connected';
        document.getElementById('connectionStatus').className = 'badge bg-success me-2';
    });

    socket.on('disconnect', () => {
        document.getElementById('connectionStatus').textContent = 'Disconnected';
        document.getElementById('connectionStatus').className = 'badge bg-danger me-2';
    });

    socket.on('boxList', (boxes) => {
        boxes.forEach(box => connectedBoxes.set(box.id, box));
        renderBoxList();
        renderDashboard();
    });

    socket.on('puzzleList', (puzzles) => {
        puzzles.forEach(puzzle => availablePuzzles.set(puzzle.id, puzzle));
        renderPuzzleList();
    });

    socket.on('boxConnected', (data) => {
        connectedBoxes.set(data.boxId, data);
        renderBoxList();
        renderDashboard();
    });

    socket.on('boxOffline', (data) => {
        if (connectedBoxes.has(data.boxId)) {
            connectedBoxes.get(data.boxId).status = 'offline';
            renderBoxList();
            renderDashboard();
        }
    });

    socket.on('boxStatus', (data) => {
        if (connectedBoxes.has(data.boxId)) {
            connectedBoxes.get(data.boxId).liveStatus = data.status;
            renderBoxStatus(data.boxId, data.status);
        }
    });
}

async function loadBoxes() {
    try {
        const response = await fetch('/api/boxes');
        const boxes = await response.json();
        boxes.forEach(box => connectedBoxes.set(box.id, box));
        renderBoxList();
        renderDashboard();
    } catch (error) {
        console.error('Failed to load boxes:', error);
    }
}

async function loadPuzzles() {
    try {
        const response = await fetch('/api/puzzles');
        const puzzles = await response.json();
        puzzles.forEach(puzzle => availablePuzzles.set(puzzle.id, puzzle));
        renderPuzzleList();
    } catch (error) {
        console.error('Failed to load puzzles:', error);
    }
}

function renderBoxList() {
    const boxList = document.getElementById('boxList');
    const boxes = Array.from(connectedBoxes.values());
    
    document.getElementById('boxCount').textContent = `${boxes.length} Boxes`;
    
    boxList.innerHTML = boxes.map(box => `
        <div class="list-group-item box-card ${box.status || 'offline'}" data-box-id="${box.id}">
            <div class="d-flex justify-content-between align-items-center">
                <div>
                    <h6 class="mb-1">${box.id}</h6>
                    <small class="text-muted">${box.ip}</small>
                </div>
                <div class="text-end">
                    <span class="badge ${box.status === 'online' ? 'bg-success' : 'bg-secondary'} status-badge">
                        ${box.status || 'offline'}
                    </span>
                    <div class="btn-group-vertical btn-group-sm mt-1">
                        <button class="btn btn-outline-primary btn-sm" onclick="showBoxDetails('${box.id}')">
                            📊 Details
                        </button>
                        <button class="btn btn-outline-warning btn-sm" onclick="resetBox('${box.id}')">
                            🔄 Reset
                        </button>
                    </div>
                </div>
            </div>
        </div>
    `).join('');
}

function renderPuzzleList() {
    const puzzleList = document.getElementById('puzzleList');
    const puzzles = Array.from(availablePuzzles.values());
    
    puzzleList.innerHTML = puzzles.map(puzzle => `
        <div class="list-group-item">
            <div class="d-flex justify-content-between align-items-center">
                <div>
                    <h6 class="mb-1">${puzzle.name}</h6>
                    <small class="text-muted">${puzzle.description || 'No description'}</small>
                    <div class="puzzle-grid mt-2">
                        ${puzzle.layout.map(type => 
                            `<div class="puzzle-cell color-${buttonColors[type]}"></div>`
                        ).join('')}
                    </div>
                </div>
                <div class="btn-group-vertical btn-group-sm">
                    <button class="btn btn-success btn-sm" onclick="showDeployModal('${puzzle.id}')">
                        🚀 Deploy
                    </button>
                    <button class="btn btn-outline-danger btn-sm" onclick="deletePuzzle('${puzzle.id}')">
                        🗑️ Delete
                    </button>
                </div>
            </div>
        </div>
    `).join('');
}

function renderDashboard() {
    const dashboardGrid = document.getElementById('dashboardGrid');
    const boxes = Array.from(connectedBoxes.values());
    
    dashboardGrid.innerHTML = boxes.map(box => `
        <div class="col-md-6 col-lg-4 mb-4">
            <div class="card">
                <div class="card-header d-flex justify-content-between align-items-center">
                    <h6 class="mb-0">${box.id}</h6>
                    <span class="badge ${box.status === 'online' ? 'bg-success' : 'bg-secondary'}">
                        ${box.status || 'offline'}
                    </span>
                </div>
                <div class="card-body text-center" id="boxStatus-${box.id}">
                    <div class="box-grid-display" id="grid-${box.id}">
                        <!-- Grid will be populated by live updates -->
                    </div>
                    <div class="corner-indicators mt-2" id="corners-${box.id}">
                        <!-- Corner indicators will be populated by live updates -->
                    </div>
                    <small class="text-muted d-block mt-2" id="lastUpdate-${box.id}">
                        No recent data
                    </small>
                </div>
            </div>
        </div>
    `).join('');
}

function renderBoxStatus(boxId, status) {
    // Update grid display
    const gridElement = document.getElementById(`grid-${boxId}`);
    if (gridElement && status.grid) {
        gridElement.innerHTML = status.grid.map((button, index) => 
            `<div class="box-grid-cell color-${buttonColors[button.type]} ${button.pressed ? 'pressed' : ''}"></div>`
        ).join('');
    }
    
    // Update corner indicators  
    const cornersElement = document.getElementById(`corners-${boxId}`);
    if (cornersElement && status.corners) {
        cornersElement.innerHTML = status.corners.map(isPressed => 
            `<div class="corner-indicator ${isPressed ? 'active' : ''}"></div>`
        ).join('');
    }
    
    // Update timestamp
    const timestampElement = document.getElementById(`lastUpdate-${boxId}`);
    if (timestampElement) {
        timestampElement.textContent = `Updated: ${new Date().toLocaleTimeString()}`;
        timestampElement.classList.add('live-status');
        setTimeout(() => timestampElement.classList.remove('live-status'), 1000);
    }
}

function createPuzzleGrid() {
    const buttonGrid = document.getElementById('buttonGrid');
    buttonGrid.innerHTML = '';
    
    for (let i = 0; i < 9; i++) {
        const select = document.createElement('select');
        select.className = 'form-select';
        select.id = `button-${i}`;
        
        buttonNames.forEach((name, index) => {
            const option = document.createElement('option');
            option.value = index;
            option.textContent = name;
            option.style.backgroundColor = `var(--bs-${buttonColors[index]}, #${buttonColors[index]})`;
            select.appendChild(option);
        });
        
        buttonGrid.appendChild(select);
    }
}

function showPuzzleCreator() {
    const modal = new bootstrap.Modal(document.getElementById('puzzleCreatorModal'));
    document.getElementById('puzzleForm').reset();
    createPuzzleGrid();
    modal.show();
}

async function savePuzzle() {
    const name = document.getElementById('puzzleName').value;
    const description = document.getElementById('puzzleDescription').value;
    const solveColor = parseInt(document.getElementById('solveColor').value);
    
    const layout = [];
    for (let i = 0; i < 9; i++) {
        layout.push(parseInt(document.getElementById(`button-${i}`).value));
    }
    
    const puzzle = {
        name,
        description,
        layout,
        solveColor
    };
    
    try {
        const response = await fetch('/api/puzzles', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(puzzle)
        });
        
        if (response.ok) {
            const savedPuzzle = await response.json();
            availablePuzzles.set(savedPuzzle.id, savedPuzzle);
            renderPuzzleList();
            bootstrap.Modal.getInstance(document.getElementById('puzzleCreatorModal')).hide();
        }
    } catch (error) {
        console.error('Failed to save puzzle:', error);
        alert('Failed to save puzzle');
    }
}

function showDeployModal(puzzleId) {
    selectedPuzzleForDeploy = puzzleId;
    const modal = new bootstrap.Modal(document.getElementById('deployModal'));
    
    // Populate box list
    const deployBoxList = document.getElementById('deployBoxList');
    const boxes = Array.from(connectedBoxes.values());
    
    deployBoxList.innerHTML = boxes.map(box => `
        <div class="form-check">
            <input class="form-check-input" type="checkbox" value="${box.id}" id="deploy-${box.id}">
            <label class="form-check-label" for="deploy-${box.id}">
                ${box.id} (${box.ip}) - ${box.status || 'offline'}
            </label>
        </div>
    `).join('');
    
    modal.show();
}

async function deployPuzzle() {
    if (!selectedPuzzleForDeploy) return;
    
    const checkboxes = document.querySelectorAll('#deployBoxList input[type="checkbox"]:checked');
    const boxIds = Array.from(checkboxes).map(cb => cb.value);
    
    if (boxIds.length === 0) {
        alert('Please select at least one box');
        return;
    }
    
    try {
        const response = await fetch('/api/deploy', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                puzzleId: selectedPuzzleForDeploy,
                boxIds
            })
        });
        
        if (response.ok) {
            bootstrap.Modal.getInstance(document.getElementById('deployModal')).hide();
            alert('Puzzle deployed successfully!');
        }
    } catch (error) {
        console.error('Failed to deploy puzzle:', error);
        alert('Failed to deploy puzzle');
    }
}

async function resetBox(boxId) {
    if (!confirm(`Reset box ${boxId}?`)) return;
    
    try {
        const response = await fetch('/api/reset', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ boxIds: [boxId] })
        });
        
        if (response.ok) {
            alert('Box reset successfully!');
        }
    } catch (error) {
        console.error('Failed to reset box:', error);
        alert('Failed to reset box');
    }
}

async function deletePuzzle(puzzleId) {
    if (!confirm('Delete this puzzle?')) return;
    
    try {
        const response = await fetch(`/api/puzzles/${puzzleId}`, {
            method: 'DELETE'
        });
        
        if (response.ok) {
            availablePuzzles.delete(puzzleId);
            renderPuzzleList();
        }
    } catch (error) {
        console.error('Failed to delete puzzle:', error);
        alert('Failed to delete puzzle');
    }
}

function refreshBoxes() {
    loadBoxes();
}