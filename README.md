# Zinc Installer
Zinc is a browser that focuses on performance and privacy and this installs Zinc for Windows users.



#### What did you use to make this installer?

We used ultralight HTML rendering engine is used to render the UI and C++ is used for the install process.



#### What is in the installation process?

1. Fetching the latest version from GitHub API
2. Download it onto your **Temp** folder
3. Unzip it
4. Move it to **%localappdata%\Programs\Zinc**
5. Add shortcuts to Start Menu
6. Attach uninstaller
7. Voila



#### How to fork Zinc Installer?

1. Clone the repository

   ```bash
   git clone https://github.com/lockieluke/ZincInstaller.git
   ```

2. Navigate into the directory

   ```bash
   cd ZincInstaller
   ```

3. Make build directory and navigate into it

   ```bash
   md build && cd build
   ```

4. Make sure you have CMake

5. Configure CMake project

   ```bash
   cmake ..
   ```

6.  Build it!

   ```bash
   cmake --config . --build Release
   ```

7. Run it!

   ```bash
   cd Release && ./ZincInstaller.exe
   ```