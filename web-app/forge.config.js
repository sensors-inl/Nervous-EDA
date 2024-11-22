const path = require('path');

module.exports = {
  packagerConfig: {
    icon: path.resolve(__dirname, 'build/icon'),
    asar: true,
  },
  rebuildConfig: {},
  makers: [
    {
      name: '@electron-forge/maker-squirrel',
      config: {
        setupIcon: path.resolve(__dirname, 'build/icon.ico'),
      }
    },
    {
      name: '@electron-forge/maker-zip'
    },
    {
      name: '@electron-forge/maker-deb',
      config: {
        options: {
          icon: path.resolve(__dirname, 'build/icon.png'),
        }
      }
    },
    {
      name: '@electron-forge/maker-rpm',
      config: {
        options: {
          icon: path.resolve(__dirname, 'build/icon.png'),
        }
      }
    },
  ],
  plugins: [
    {
      name: '@electron-forge/plugin-auto-unpack-natives',
      config: {},
    },
  ],
};
