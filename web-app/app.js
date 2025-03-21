/* Copyright (c) 2024 INSA Lyon, CNRS, INL UMR 5270 */
/* This file is under MIT Licence */
/* Full text available at https://mit-license.org/ */

/* Compatibility check */
const userAgent = navigator.userAgent.toLowerCase();
console.log("Application is runnning on " + userAgent);
const isSafari = /^((?!chrome|android).)*safari/i.test(userAgent);
const isCompatible = (/chrome|edg|opera/i.test(userAgent)) && !isSafari;
if (!isCompatible) {
  alert("Applications require Chrome, Edge or Opera web browsers");
}

/*******************************************************************************
 * BLE connection and RX data handler
 ******************************************************************************/

/* Graphical components binding */
const connectBLEButton = document.querySelector('.app-ble-connect-button');
const connectBLEButtonRipple = new mdc.ripple.MDCRipple(connectBLEButton);
const connectBLEButtonLabel = document.querySelector('.app-ble-connect-button-label');
const connectBLEStatusIcon = document.querySelector('.app-ble-status-icon');

/* Global variables */
const UUIDS = {
    NRF_UART_SERVICE_UUID: '6e400001-b5a3-f393-e0a9-e50e24dcca9e',
    NRF_UART_TX_CHAR_UUID: '6e400002-b5a3-f393-e0a9-e50e24dcca9e',
    NRF_UART_RX_CHAR_UUID: '6e400003-b5a3-f393-e0a9-e50e24dcca9e',
};
const bleDeviceNamePrefix = "EDA"
let bleDevice;
let bleDeviceName;
let bleConnected = false;


async function onConnectBLEButtonClick() {
    if (bleConnected == false) {
        disableAllButtons();
        connectBLEButtonLabel.innerHTML = 'Connecting...'
        let searchTimer = setInterval(
            function () {
                if (connectBLEStatusIcon.innerHTML == 'bluetooth_searching') {
                    connectBLEStatusIcon.innerHTML = 'bluetooth';
                }
                else {
                    connectBLEStatusIcon.innerHTML = 'bluetooth_searching';
                }
            },
            750
        );
        try {
            bleDevice = await navigator.bluetooth.requestDevice({
                filters: [
                    { namePrefix: bleDeviceNamePrefix },
                ],
                optionalServices: [UUIDS.NRF_UART_SERVICE_UUID, 'device_information', 'battery_service'],
            });
            bleDevice.addEventListener('gattserverdisconnected', bleOnDisconnected);
            const gatt = await bleDevice.gatt.connect();
            // Get DIS characteristics
            const dis = await gatt.getPrimaryService('device_information');
            const versionChar = await dis.getCharacteristic('firmware_revision_string');
            const versionValue = await versionChar.readValue();
            let decoder = new TextDecoder('utf-8');
            const versionValueString = decoder.decode(versionValue);
            // Get BAS characteristics
            const batService = await gatt.getPrimaryService('battery_service');
            const batChar = await batService.getCharacteristic('battery_level');
            const nusService = await gatt.getPrimaryService(UUIDS.NRF_UART_SERVICE_UUID);
            const rxChar = await nusService.getCharacteristic(UUIDS.NRF_UART_RX_CHAR_UUID);
            const txChar = await nusService.getCharacteristic(UUIDS.NRF_UART_TX_CHAR_UUID);
            bleSetupRxListener(rxChar);
            bleSetupBatListener(batChar);
            window.rxChar = rxChar;
            window.txChar = txChar;
            window.batChar = batChar;
            window.batChar.startNotifications();
            console.log("Connected");
            bleConnected = true;
            clearInterval(searchTimer);
            enableControlButtons();
            connectBLEButtonLabel.innerHTML = 'Disconnect';
            connectBLEStatusIcon.innerHTML = 'bluetooth_connected';
            connectBLEButton.removeAttribute('disabled');
            connectBLEStatusIcon.removeAttribute('disabled');
            bleDeviceName = bleDevice.name;
            deviceLabel.innerHTML = 'Device: ' + bleDeviceName + " v" + versionValueString;;
            getDeviceInformation();
        }
        catch (err) {
            console.error("BLE Connection error " + err);
            alert("Unable to connect");
            clearInterval(searchTimer);
            connectBLEButtonLabel.innerHTML = 'Connect';
            connectBLEStatusIcon.innerHTML = 'bluetooth';
            connectBLEButton.removeAttribute('disabled');
        }
    }
    else {
        bleDevice.gatt.disconnect();
    }
}

function bleOnDisconnected(event) {
    console.log("Disconnected");
    console.log(event);
    bleConnected = false;
    disableAllButtons();
    connectBLEButtonLabel.innerHTML = 'Connect';
    connectBLEStatusIcon.innerHTML = 'bluetooth';
    connectBLEButton.removeAttribute('disabled');
    connectBLEStatusIcon.setAttribute('disabled', '');
    deviceLabel.innerHTML = 'Device';
}

function bleSetupRxListener(rxchar) {
    let rx_buf = [];

    /** This is the main RX callback for NUS. It handles sliced messages before calling appropriate callback */
    rxchar.addEventListener("characteristicvaluechanged",
        /** @param {Bluetooth} e */
        (e) => {
            /** @type {BluetoothRemoteGATTCharacteristic} */
            const char = e.target;
            let rx_data = new Uint8Array(char.value.buffer);
            rx_buf = new Uint8Array([...rx_buf,...rx_data]);
            let zeroIndex = rx_buf.indexOf(0);
            if(zeroIndex != -1) {
                const cobs_data = rx_buf.slice(0, zeroIndex + 1);
                decodeMessage(cobs_data);
                rx_buf = rx_buf.slice(zeroIndex + 1);
            }

        }
    );
}

function bleSetupBatListener(batchar) {
    batchar.addEventListener("characteristicvaluechanged",
        /** @param {Bluetooth} e */
        (e) => {
            /** @type {BluetoothRemoteGATTCharacteristic} */
            const char = e.target;
            const rx_data = new Uint8Array(char.value.buffer);
            const batteryLevel = rx_data[0];
            console.log("Battery " + batteryLevel +"%");
            updateViewBattery(batteryLevel);
        }
    );
}

/*******************************************************************************
 * RX Message decoder
 ******************************************************************************/

/**
 * @param {Uint8Array} message
 */
function decodeMessage(message) {
    try {
        const decoded = decode(message).subarray(0,-1);
        const edaBuffer = proto.EdaBuffer.deserializeBinary(decoded);
        const measures = edaBuffer.getDataList();
        const timestamp = edaBuffer.getTimestamp();
        const time = (timestamp.getTime() + (timestamp.getUs() * 10**-6)) - timeDataStart;
        nyquistChartAddResults(measures, time);
    } catch (error) {
        console.error("Error while decoding message: " + error);
    }
}

/*******************************************************************************
 * TX Message encoder
 ******************************************************************************/

/**
 * @param {proto.Timestamp} timestamp
 */
async function encodeMessage(timestamp) {
    /* Serialize JS object */
    let protoBuffer = timestamp.serializeBinary();
    /* Encode with COBS */
    let cobsBuffer = encode(protoBuffer);
    /* Add final zero for subsequent decoding by nanocobs */
    cobsBuffer = new Uint8Array([...cobsBuffer, 0]);
    /* Dispatch to interface */
    if (bleConnected == true) {
        await txChar.writeValueWithoutResponse(cobsBuffer);
    }
    else {
        console.error("No device connected to send request");
    }
}

/*******************************************************************************
 * Crossed interface controls
 ******************************************************************************/

/* Graphical components binding */
const startMeasureButton = document.querySelector('.app-start-measure-button');
const startMeasureButtonRipple = new mdc.ripple.MDCRipple(startMeasureButton);
const stopMeasureButton = document.querySelector('.app-stop-measure-button');
const stopMeasureButtonRipple = new mdc.ripple.MDCRipple(stopMeasureButton);
const batteryStatusIcon = document.querySelector('.app-bat-status-icon');
const deviceLabel = document.getElementById('device-title-id');

function disableControlButtons() {
    startMeasureButton.setAttribute('disabled', '');
    stopMeasureButton.setAttribute('disabled', '');
    batteryStatusIcon.setAttribute('disabled', '');
}

function enableControlButtons() {
    startMeasureButton.removeAttribute('disabled');
    stopMeasureButton.removeAttribute('disabled');
    batteryStatusIcon.removeAttribute('disabled');
}

function disableAllButtons() {
    disableControlButtons();
    connectBLEButton.setAttribute('disabled', '');
}

/**
 * @param {number} level 
 */
function updateViewBattery(level) {
    if (level > 90) {
        batteryStatusIcon.innerHTML = 'battery_full';
    }
    else if (level > 80) {
        batteryStatusIcon.innerHTML = 'battery_6_bar';
    }
    else if (level > 70) {
        batteryStatusIcon.innerHTML = 'battery_5_bar';
    }
    else if (level > 60) {
        batteryStatusIcon.innerHTML = 'battery_4_bar';
    }
    else if (level > 50) {
        batteryStatusIcon.innerHTML = 'battery_3_bar';
    }
    else if (level > 40) {
        batteryStatusIcon.innerHTML = 'battery_2_bar';
    }
    else if (level > 30) {
        batteryStatusIcon.innerHTML = 'battery_1_bar';
    }
    else if (level > 20) {
        batteryStatusIcon.innerHTML = 'battery_0_bar';
    }
    else {
        batteryStatusIcon.innerHTML = 'battery_alert';
    }
}

async function onStartMeasureButtonClick() {
    if (bleConnected == false) return;
    // Save start time
    let millis = Date.now();
    timeDataStart = millis * 0.001;
    // Placeholder to save data in file later
    window.data = [];
    // Enable notifications
    window.rxChar.startNotifications();
    // Reset graphes
    onClearGraphClick();
}

async function onStopMeasureButtonClick() {
    if (bleConnected == false) return;
    // Disable notifications
    window.rxChar.stopNotifications();
    // Save data
    saveWindowData();
}

async function saveCurrentData() {
    // Save data
    saveWindowData();
}

async function setDeviceTimestamp() {
    const date = Date.now();
    const seconds = Math.floor(Date.now() * 1e-3);
    const micros = (date - (seconds * 1e3)) * 1e3;
    const timestamp = new proto.Timestamp()
        .setTime(seconds)
        .setUs(micros);
    await encodeMessage(timestamp);
}

async function getDeviceBattery() {
    await batChar.readValue().then(value => {
        let batteryLevel = value.getUint8(0);
        console.log("Battery " + batteryLevel +"%");
        updateViewBattery(batteryLevel);
    })
}

function getDeviceInformation() {
    setTimeout(setDeviceTimestamp, 200);
    setTimeout(getDeviceBattery, 400);
}

/**
 * @param {number} timestamp
 * @returns {string} formattedTime
 */
function getFormattedTime(timestamp) {
    let date = new Date(timestamp * 1000);
    let year = date.getFullYear();
    let month = "0" + (date.getMonth() + 1);
    let day = "0" + date.getDate();
    let hours = "0" + date.getHours();
    let minutes = "0" + date.getMinutes();
    let seconds = "0" + date.getSeconds();
    let formattedTime = year + '-' + month.substr(-2) + '-' + day.substr(-2) + ' ' + hours.substr(-2) + ':' + minutes.substr(-2) + ':' + seconds.substr(-2);
    return formattedTime;
}

/*******************************************************************************
 * Data Plots
 ******************************************************************************/

/*** Global variables ***/
Chart.defaults.font.family = "'Roboto', 'Verdana'";
Chart.defaults.font.size = 12;
const chartTimeMax = 60.0; // seconds
let timeDataStart = 0.0;
const colorset = [ '#ef5350FF', '#5c6bc0FF', '#26a69aFF', '#ffee58FF', '#5d4037ff', '#e91e63ff', '#2196f3ff', '#4caf50ff', '#ffc107ff', '#ef5350FF', '#5c6bc0FF', '#26a69aFF', '#ffee58FF', '#5d4037ff', '#e91e63ff', '#2196f3ff', '#4caf50ff', '#ffc107ff'];
const EDA_FREQUENCY_LIST = [12, 28, 32, 36, 44, 68, 84, 108, 136, 196, 256, 324, 400, 484, 576, 724];
window.data = [];

/* Graphical components binding */

const nyquistChart = new Chart(document.getElementById('app-nyquist-chart-canvas'), {
    type: 'scatter',
    data: {
        labels: [],
        datasets: [],
    },
    options: {
        responsive: true,
        aspectRatio: 1,
        maintainAspectRatio: false,
        scales: {
            x: { // defining min and max so hiding the dataset does not change scale range
                suggestedMin: -10000,
                suggestedMax: 90000,
            },
            y: { // defining min and max so hiding the dataset does not change scale range
                suggestedMin: -40000,
                suggestedMax: 10000,
                reverse: true,
            },
        },
        plugins: {
            tooltip: {
                callbacks: {
                    label: function (context) {
                        var label = context.dataset.labels[context.dataIndex];
                        return label + ' Hz: (' + context.raw.x + ', ' + context.raw.y + ')';
                    }
                }
            },
            legend: {
                display: false
            },
            zoom: {
                zoom: {
                    wheel: {
                        enabled: false,
                    },
                    pinch: {
                        enabled: false
                    },
                    mode: 'xy',
                },
            }
        }
    }
});

const magnitudeChart = new Chart(document.getElementById('app-magnitude-chart-canvas'), {
    type: 'scatter',
    data: {
        labels: [],
        datasets: [],
    },
    options: {
        pointRadius: 0,
        animation: {
            duration: 0,
        },
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            x: { // defining min and max so hiding the dataset does not change scale range
                suggestedMin: 0,
                suggestedMax: chartTimeMax,
            },
            y: { // defining min and max so hiding the dataset does not change scale range
                suggestedMin: 0,
                suggestedMax: 2,
            },
        },
    }
});

const historyChart = new Chart(document.getElementById('app-history-chart-canvas'), {
    type: 'scatter',
    data: {
        labels: [],
        datasets: [],
    },
    options: {
        pointRadius: 0,
        animation: {
            duration: 0,
        },
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            zoom: {
                zoom: {
                    wheel: {
                        enabled: true,
                    },
                    pinch: {
                        enabled: true
                    },
                    mode: 'x',
                },
                pan: {
                    enabled: true,
                    mode: 'x'
                }
            }
        },
        scales: {
            x: { // defining min and max so hiding the dataset does not change scale range
                suggestedMin: 0,
                suggestedMax: chartTimeMax,
            },
            y: { // defining min and max so hiding the dataset does not change scale range
                suggestedMin: 0,
                suggestedMax: 2,
            },
        },
    }
});

/*** Helpers ***/

function onClearGraphClick() {
    nyquistChart.data.datasets = [];
    nyquistChart.update();

    magnitudeChart.data.labels = [];
    magnitudeChart.data.datasets = [];
    magnitudeChart.update();

    historyChart.data.labels = [];
    historyChart.data.datasets = [];
    historyChart.update();

    window.data = [];
}

/**
 * 
 * @param {!Array<!proto.Impedance>} data 
 * @param {number} time 
 */
function nyquistChartAddResults(data, time) {
    const results = data.map((element, index) => {
        var val = {};
        val.real = element.getReal();
        val.imag = element.getImag();
        val.freq = EDA_FREQUENCY_LIST[index];
        return val;
    });

    const impedanceData = results.map(
        function (element) {
            return { 'x': element.real, 'y': element.imag };
        }
    );
    let labels = results.map(element => element.freq);
    let circle = {
        points: [],
        x: 0.0,
        y: 0.0,
        r: 0.0
    };
    try {
        circle = circleEstimatorJSX(impedanceData);
    }
    catch (err) {
        console.log(err);
    }

    let circleData = circle.points;
    if (nyquistChart.data.datasets.length == 0) {
        nyquistChart.data.datasets = [
            {
                label: '',
                labels: labels,
                data: impedanceData,
                tension: 0.4,
                backgroundColor: colorset[0],
                borderColor: colorset[0],
                showLine: false,
            },
            {
                label: '',
                labels: [],
                data: circleData,
                tension: 0.4,
                backgroundColor: colorset[1],
                borderColor: colorset[1],
                showLine: true,
                borderWidth: 1,
                borderDash: [10, 10],
                pointRadius: 0,
            },
        ];
    } else {
        nyquistChart.data.datasets[0].data.forEach((element, index) => {
            element.x = impedanceData[index].x;
            element.y = impedanceData[index].y;
        });
        if (circleData.length > 0) {
            nyquistChart.data.datasets[1].data.forEach((element, index) => {
                element.x = circleData[index].x;
                element.y = circleData[index].y;
            });
        }
    }
    nyquistChart.update();

    /* Update conductance chart */
    for (let i = 0; i < EDA_FREQUENCY_LIST.length; i++) {
        if (magnitudeChart.data.datasets.length < EDA_FREQUENCY_LIST.length) {
            if (i == 0) {
                magnitudeChart.data.datasets.push({ label: EDA_FREQUENCY_LIST[i] + " Hz", labels: [], data: [], tension: 0.4, backgroundColor: colorset[i], borderColor: colorset[i], showLine: true });
            }
            else {
                magnitudeChart.data.datasets.push({ label: EDA_FREQUENCY_LIST[i] + " Hz", labels: [], data: [], hidden: true, tension: 0.4, backgroundColor: colorset[i], borderColor: colorset[i], showLine: true });
            }
        }
        magnitudeChart.data.datasets[i].data.push({x: time, y:(1000000.0/Math.sqrt((impedanceData[i].x ** 2) + (impedanceData[i].y ** 2)))});
    }

    while (magnitudeChart.data.datasets[0].data[magnitudeChart.data.datasets[0].data.length - 1].x - magnitudeChart.data.datasets[0].data[0].x > chartTimeMax) {
        //magnitudeChart.data.labels.shift();
        for (let i = 0; i < EDA_FREQUENCY_LIST.length; i++) {
            magnitudeChart.data.datasets[i].data.shift();
        }
    }
    magnitudeChart.options.scales['x'].min = Math.floor(magnitudeChart.data.datasets[0].data[0].x);
    magnitudeChart.options.scales['x'].max = Math.max(chartTimeMax, Math.ceil(magnitudeChart.data.datasets[0].data[magnitudeChart.data.datasets[0].data.length - 1].x));

    /* Save to global window data storage for file storage */
    let dataArray = [];
    dataArray.push(time)
    impedanceData.forEach(element => {
        dataArray.push(element.x);
        dataArray.push(element.y);
    });
    dataArray.push(circle.x);
    dataArray.push(circle.y);
    dataArray.push(circle.r);
    window.data.push(dataArray);
}


setInterval(refreshMagnitudeChart, 1000)

function refreshMagnitudeChart() {
    magnitudeChart.update();
}

setInterval(refreshHistoryChart, 60000)

function refreshHistoryChart() {
    historyChart.data.labels = [];
    historyChart.data.datasets = [];
    historyChart.data.datasets.push({ label: EDA_FREQUENCY_LIST[0] + " Hz", labels: [], data: [], tension: 0.4, backgroundColor: colorset[0], borderColor: colorset[0], showLine: true });
    window.data.forEach(dataArray => {
        real = dataArray[1];
        imag = dataArray[2];
        historyChart.data.datasets[0].data.push({x:dataArray[0], y:1000000.0/Math.sqrt((real ** 2) + (imag ** 2))});
    });
    historyChart.update();
}

function saveWindowData() {
    refreshHistoryChart();
    // Set filename
    let date = getFormattedTime(timeDataStart);
    date = date.replace(/:/g, "-").replace(/ /g, "_");
    const filename = bleDeviceName +"_" + date + '.csv';

    if (nyquistChart.data.datasets.length > 0) {
        // Set header
        const freqs = nyquistChart.data.datasets[0].labels;
        let header = 'Time(s), ';
        freqs.forEach(e => {
            header = header + e.toString() + 'Hz(Re), ' + e.toString() + 'Hz(Im), ';
        })
        header = header + 'Cx, Cy, Cr\r\n';
        let csvContent = header + //'23Hz(Re)\t23Hz(Im)\t37Hz(Re)\t37Hz(Im)\t59Hz(Re)\t59Hz(Im)\t83Hz(Re)\t83Hz(Im)\t103Hz(Re)\t103Hz(Im)\t127Hz(Re)\t127Hz(Im)\t157Hz(Re)\t157Hz(Im)\t191Hz(Re)\t191Hz(Im)\t211Hz(Re)\t211Hz(Im)\tCx\tCy\tCr\r\n' + 
            window.data.map(e =>
                e.map(e => e.toFixed(4).toString()).join(', ')
            ).join('\r\n');
        
        let element = document.createElement('a');
        element.setAttribute('href', 'data:text;charset=utf-8,' + encodeURIComponent(csvContent));
        element.setAttribute('download', filename);
        element.style.display = 'none';
    
        document.body.appendChild(element);
        element.click();
        document.body.removeChild(element);
    }
}

/*******************************************************************************
 * Circle data fitter
 ******************************************************************************/

function circleEstimatorJSX(p) {
    var i;

    // Having constructed the points, we can fit a circle 
    // through the point set, consisting of n points.
    // The (n times 3) matrix consists of
    //   x_1, y_1, 1
    //   x_2, y_2, 1
    //      ...
    //   x_n, y_n, 1
    // where x_i, y_i is the position of point p_i
    // The vector y of length n consists of
    //    x_i*x_i+y_i*y_i 
    // for i=1,...n.
    var M = [], y = [], MT, B, c, z, n;
    n = p.length;
    for (i = 0; i < n; i++) {
        M.push([p[i].x, p[i].y, 1.0]);
        y.push(p[i].x * p[i].x + p[i].y * p[i].y);
    }

    // Now, the general linear least-square fitting problem
    //    min_z || M*z - y||_2^2
    // is solved by solving the system of linear equations
    //    (M^T*M) * z = (M^T*y)
    // with Gauss elimination.
    MT = JXG.Math.transpose(M);
    B = JXG.Math.matMatMult(MT, M);
    c = JXG.Math.matVecMult(MT, y);
    z = JXG.Math.Numerics.Gauss(B, c);

    // Finally, we can read from the solution vector z the coordinates [xm, ym] of the center
    // and the radius r and draw the circle.
    var xm = z[0] * 0.5;
    var ym = z[1] * 0.5;
    var r = Math.sqrt(z[2] + xm * xm + ym * ym);

    var circleData = [];
    for (i = 0; i >= -Math.PI; i -= Math.PI / 20) {
        circleData.push({
            x: xm + (r * Math.cos(i)),
            y: ym + (r * Math.sin(i)),
        });
    }
    var circle = {
        points: [],
        x: 0.0,
        y: 0.0,
        r: 0.0
    };
    circle.points = circleData;
    circle.x = xm;
    circle.y = ym;
    circle.r = r;
    return circle;
}

console.log("RUN JAVASCRIPT,  RUUUUUUUUNNNNN !!!!!");