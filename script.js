const API_URL = "http://localhost:8080/api";

// Sync C++ Server Status Every 1 Second
async function syncWithCppEngine() {
  try {
    const res = await fetch(`${API_URL}/status`);
    const data = await res.json();

    renderSlots('slots-container', data.regular, 'Slot');
    renderSlots('vip-slots-container', data.vip, 'VIP');
    renderQueue(data.queue);
    updateStats(data.regular, data.vip, data.stackSize);
  } catch (err) {
    document.getElementById('log-container').innerHTML = 
      `<div style="color: #ef4444;">[SERVER OFFLINE] C++ Backend Server is NOT running! Run main.cpp in Terminal!</div>`;
  }
}

function renderSlots(containerId, slots, prefix) {
  const container = document.getElementById(containerId);
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
  if (!queueArr || queueArr.length === 0) {
    container.innerText = "Queue is empty";
  } else {
    container.innerHTML = queueArr.map(car => `<span class="queue-tag">${car}</span>`).join(' ➔ ');
  }
}

function updateStats(reg, vip, stackSize) {
  let occReg = reg.filter(s => s !== "FREE").length;
  let occVip = vip.filter(s => s !== "FREE").length;
  let totalOcc = occReg + occVip;

  document.getElementById('stat-total').innerText = "15";
  document.getElementById('stat-occupied').innerText = totalOcc;
  document.getElementById('stat-free').innerText = 15 - totalOcc;
  document.getElementById('stat-stack').innerText = `${stackSize} Action(s)`;
}

// 1. ENTRY HANDLER
async function handleEntry() {
  const plateInput = document.getElementById('plate-input');
  const plate = plateInput.value.trim().toUpperCase();
  const type = document.getElementById('type-select').value;
  
  if (!plate) return alert("Please enter a License Plate number!");

  try {
    const res = await fetch(`${API_URL}/entry?plate=${plate}&type=${type}`, { method: 'POST' });
    const result = await res.json();
    addLog(`[C++ API RESULT] Vehicle ${plate} allocated to ${result.location}`);
    plateInput.value = '';
    syncWithCppEngine();
  } catch (e) {
    alert("Error connecting to server!");
  }
}

// 2. EXIT HANDLER
async function handleExit() {
  const plateInput = document.getElementById('plate-input');
  const plate = plateInput.value.trim().toUpperCase();
  
  if (!plate) return alert("Please enter a License Plate number!");

  try {
    const res = await fetch(`${API_URL}/exit?plate=${plate}`, { method: 'POST' });
    const result = await res.json();

    if (result.status === "success") {
      addLog(`[C++ API RESULT] Vehicle ${plate} ${result.message}`);
    } else {
      addLog(`[C++ API ERROR] ${result.message}`);
    }

    plateInput.value = '';
    syncWithCppEngine();
  } catch (e) {
    alert("Error connecting to server!");
  }
}

// 3. STACK UNDO HANDLER (LIFO)
async function handleUndo() {
  try {
    const res = await fetch(`${API_URL}/undo`, { method: 'POST' });
    const result = await res.json();

    if (result.status === "success") {
      addLog(`[C++ STACK UNDO (LIFO)] ${result.message}`);
    } else {
      addLog(`[C++ STACK ERROR] ${result.message}`);
    }
    syncWithCppEngine();
  } catch (e) {
    alert("Error executing Undo action!");
  }
}

// 4. HASH MAP O(1) SEARCH HANDLER
async function handleSearch() {
  const searchInput = document.getElementById('search-input');
  const plate = searchInput.value.trim().toUpperCase();
  const resDiv = document.getElementById('search-result');
  
  if (!plate) return alert("Enter License Plate to Search!");

  try {
    const res = await fetch(`${API_URL}/search?plate=${plate}`);
    const result = await res.json();

    if (result.found) {
      resDiv.innerText = `[C++ HASH MAP O(1)] FOUND: ${plate} is in ${result.location}`;
    } else {
      resDiv.innerText = `[C++ HASH MAP O(1)] NOT FOUND: ${plate} not in memory.`;
    }
  } catch (e) {
    resDiv.innerText = "Error searching vehicle!";
  }
}

function addLog(msg) {
  const logBox = document.getElementById('log-container');
  const time = new Date().toLocaleTimeString();
  logBox.innerHTML = `<div>[${time}] ${msg}</div>` + logBox.innerHTML;
}

// Initial Sync
document.addEventListener('DOMContentLoaded', () => {
  addLog("Connected to C++ Backend Server Engine.");
  syncWithCppEngine();
  setInterval(syncWithCppEngine, 1000);
});