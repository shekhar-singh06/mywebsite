// --- BACKEND API CONFIGURATION --- //
const API_BASE_URL = "https://mywebsite-o7vb.onrender.com";

// --- 1. REAL-TIME DATA SYNC ENGINE --- //
async function syncWithCppEngine() {
    try {
        const res = await fetch(`${API_BASE_URL}/api/status`);
        const data = await res.json();

        renderSlots('slots-container', data.regular, 'Slot');
        renderSlots('vip-slots-container', data.vip, 'VIP');
        renderQueue(data.queue);
        updateStats(data.regular, data.vip, data.stackSize);
    } catch (err) {
        const logContainer = document.getElementById('log-container');
        if (logContainer) {
            logContainer.innerHTML = 
                `<div style="color: #ef4444; font-weight: bold;">[SERVER OFFLINE] Connecting to C++ Backend...</div>`;
        }
    }
}

// --- 2. UI RENDER FUNCTIONS --- //
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

// --- 3. REST API CALLS --- //

// Vehicle Entry
async function addVehicle(plate, type) {
    try {
        const response = await fetch(`${API_BASE_URL}/api/entry?plate=${encodeURIComponent(plate)}&type=${encodeURIComponent(type)}`, {
            method: 'POST'
        });
        const data = await response.json();
        if (data.status === "success") {
            const plateInput = document.querySelector("input[placeholder*='UK07']") || document.querySelector("input[type='text']");
            if (plateInput) plateInput.value = "";
            await syncWithCppEngine();
        } else {
            alert(data.message || "Error adding vehicle");
        }
    } catch (err) {
        alert("Error connecting to server!");
    }
}

// Vehicle Exit
async function removeVehicle(plate) {
    try {
        const response = await fetch(`${API_BASE_URL}/api/exit?plate=${encodeURIComponent(plate)}`, {
            method: 'POST'
        });
        const data = await response.json();
        if (data.status === "success") {
            const plateInput = document.querySelector("input[placeholder*='UK07']") || document.querySelector("input[type='text']");
            if (plateInput) plateInput.value = "";
            await syncWithCppEngine();
        } else {
            alert(data.message || "Vehicle not found");
        }
    } catch (err) {
        alert("Error connecting to server!");
    }
}

// Undo Last Action (LIFO Stack)
async function undoLastAction() {
    try {
        const response = await fetch(`${API_BASE_URL}/api/undo`, {
            method: 'POST'
        });
        const data = await response.json();
        alert(data.message || "Undo action executed!");
        await syncWithCppEngine();
    } catch (err) {
        alert("Error performing undo!");
    }
}

// Search Vehicle O(1)
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

// --- 4. EXPLICIT PREVENT-DEFAULT BINDINGS --- //
document.addEventListener("DOMContentLoaded", () => {
    syncWithCppEngine();
    setInterval(syncWithCppEngine, 3000);

    // Stop all forms from submitting automatically
    document.querySelectorAll("form").forEach(form => {
        form.onsubmit = (e) => e.preventDefault();
    });

    document.querySelectorAll("button").forEach(btn => {
        btn.type = "button"; // Force type='button' to prevent form submit

        const txt = btn.innerText.toLowerCase().trim();

        if (txt.includes("undo")) {
            btn.onclick = (e) => {
                e.preventDefault();
                e.stopPropagation();
                undoLastAction();
            };
        } 
        else if (txt.includes("entry")) {
            btn.onclick = (e) => {
                e.preventDefault();
                e.stopPropagation();
                const plateInput = document.querySelector("input[placeholder*='UK07']") || document.querySelector("input[type='text']");
                const typeSelect = document.querySelector("select");
                
                const plate = plateInput ? plateInput.value.trim() : "";
                const type = typeSelect ? typeSelect.value : "Regular Vehicle";

                if (!plate) return alert("Please enter License Plate!");
                addVehicle(plate, type.toUpperCase().includes("VIP") ? "VIP" : "REGULAR");
            };
        } 
        else if (txt.includes("exit")) {
            btn.onclick = (e) => {
                e.preventDefault();
                e.stopPropagation();
                const plateInput = document.querySelector("input[placeholder*='UK07']") || document.querySelector("input[type='text']");
                const plate = plateInput ? plateInput.value.trim() : "";

                if (!plate) return alert("Please enter License Plate!");
                removeVehicle(plate);
            };
        } 
        else if (txt.includes("search")) {
            btn.onclick = (e) => {
                e.preventDefault();
                e.stopPropagation();
                const inputs = document.querySelectorAll("input[type='text']");
                const searchInput = inputs.length > 1 ? inputs[1] : inputs[0];
                const plate = searchInput ? searchInput.value.trim() : "";

                if (!plate) return alert("Enter Plate Number to Search!");
                searchVehicle(plate);
            };
        }
    });
});
