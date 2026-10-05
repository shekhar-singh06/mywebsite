const API_BASE_URL = "https://mywebsite-o7vb.onrender.com";

async function syncWithCppEngine() {
    try {
        const res = await fetch(`${API_BASE_URL}/api/status`);
        const data = await res.json();

        renderSlots('slots-container', data.regular, 'Slot');
        renderSlots('vip-slots-container', data.vip, 'VIP');
        renderQueue(data.queue);
        updateStats(data.regular, data.vip, data.stackSize);
    } catch (err) {
        document.getElementById('log-container').innerHTML = 
            `<div style="color: #ef4444;">[SERVER OFFLINE] C++ Backend Server is NOT running!</div>`;
    }
}

function renderSlots(containerId, slots, prefix) {
    const container = document.getElementById(containerId);
    if (!container || !slots) return;
    container.innerHTML = '';
    slots.forEach((val, index) => {
        const isOccupied = val !== "FREE";
        container.innerHTML += `
            <div class="slot-box ${prefix === 'VIP' ? 'vip-slot' : ''} ${isOccupied ? 'occupied' : ''}">
                ${prefix} ${index + 1}<br>
                <small>${isOccupied ? val : 'FREE'}</small>
            </div>`;
    });
}

function renderQueue(queueArr) {
    const container = document.getElementById('queue-container');
    if (!container) return;
    if (!queueArr || queueArr.length === 0) {
        container.innerText = "Queue is empty";
    } else {
        container.innerHTML = queueArr.map(car => `<span class="queue-tag">${car}</span>`).join(' ');
    }
}

function updateStats(reg, vip, stackSize) {
    if (!reg || !vip) return;
    let occReg = reg.filter(s => s !== "FREE").length;
    let occVip = vip.filter(s => s !== "FREE").length;
    let totalOcc = occReg + occVip;

    const elTotal = document.getElementById('stat-total');
    const elOcc = document.getElementById('stat-occupied');
    const elFree = document.getElementById('stat-free');
    const elActions = document.getElementById('stat-actions');

    if (elTotal) elTotal.innerText = "15";
    if (elOcc) elOcc.innerText = totalOcc;
    if (elFree) elFree.innerText = 15 - totalOcc;
    if (elActions) elActions.innerText = stackSize !== undefined ? stackSize : 0;
}

// --- GATE OPERATIONS & REST API CALLS --- //

// 1. ADD VEHICLE ENTRY
async function addVehicle(plate, type) {
    try {
        const response = await fetch(`${API_BASE_URL}/api/entry?plate=${encodeURIComponent(plate)}&type=${encodeURIComponent(type)}`, {
            method: 'POST'
        });
        const data = await response.json();
        if (data.status === "success") {
            syncWithCppEngine();
        } else {
            alert(data.message || "Error adding vehicle");
        }
    } catch (err) {
        alert("Error connecting to server!");
    }
}

// 2. VEHICLE EXIT
async function removeVehicle(plate) {
    try {
        const response = await fetch(`${API_BASE_URL}/api/exit?plate=${encodeURIComponent(plate)}`, {
            method: 'POST'
        });
        const data = await response.json();
        if (data.status === "success") {
            syncWithCppEngine();
        } else {
            alert(data.message || "Vehicle not found");
        }
    } catch (err) {
        alert("Error connecting to server!");
    }
}

// 3. UNDO LAST ENTRY (STACK LIFO)
async function undoLastAction() {
    try {
        const response = await fetch(`${API_BASE_URL}/api/undo`, {
            method: 'POST'
        });
        const data = await response.json();
        alert(data.message || "Undo action executed!");
        syncWithCppEngine();
    } catch (err) {
        alert("Error performing undo!");
    }
}

// 4. INSTANT SEARCH O(1)
async function searchVehicle(plate) {
    try {
        const response = await fetch(`${API_BASE_URL}/api/search?plate=${encodeURIComponent(plate)}`);
        const data = await response.json();
        if (data.found) {
            alert(`Vehicle ${plate} is parked at: ${data.location}`);
        } else {
            alert(`Vehicle ${plate} not found in Hash Map!`);
        }
    } catch (err) {
        alert("Search request failed!");
    }
}

// Auto sync state on page load and loop every 3 seconds
document.addEventListener("DOMContentLoaded", () => {
    syncWithCppEngine();
    setInterval(syncWithCppEngine, 3000);
});
