const electron = require('electron')
const { app, BrowserWindow, ipcMain } = require('electron')
const path = require('path')

// run this as early in the main process as possible
if (require('electron-squirrel-startup')) app.quit();

function createWindow () {
    const mainWindow = new BrowserWindow({
      width: 1280,
      height: 830,
      minWidth: 1280,
      minHeight: 830,
      icon: path.join(__dirname, 'build/icon.png')
    })
    
    mainWindow.removeMenu()

    mainWindow.webContents.on('select-bluetooth-device', (event, deviceList, callback) => {
      console.log(deviceList)
      event.preventDefault()
      const result = deviceList.find((device) => {
        return true
      })
      if (result) {
        callback(result.deviceId)
      } else {
        // The device wasn't found so we need to either wait longer (eg until the
        // device is turned on) or until the user cancels the request
      }
    })
  
    ipcMain.on('cancel-bluetooth-request', (event) => {
    })
  
    // Listen for a message from the renderer to get the response for the Bluetooth pairing.
    ipcMain.on('bluetooth-pairing-response', (event, response) => {
    })
  
    mainWindow.webContents.session.setBluetoothPairingHandler((details, callback) => {
      // Send a message to the renderer to prompt the user to confirm the pairing.
      mainWindow.webContents.send('bluetooth-pairing-request', details)
    })
  
    mainWindow.loadFile('index.html')
  }

  app.whenReady().then(() => {
    createWindow()
  
    app.on('activate', function () {
      if (BrowserWindow.getAllWindows().length === 0) createWindow()
    })
  })
  
  app.on('window-all-closed', function () {
    if (process.platform !== 'darwin') app.quit()
  })