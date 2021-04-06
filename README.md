# mosquitto-disconnections-plugin
Simple mosquitto plugins that notifies client disconnection events to a given topic

# Dev env setup
1. Donwload Mosquitto source
    ```bash
    git clone https://github.com/eclipse/mosquitto
    ```
1. Prepare the plugin directory where to store this plugin source files
    ```bash
    mkdir mosquitto/plugins/disconnections
    ```
1. Copy the contents of this repo's src folder into mosquitto/plugins/disconnections
    ```bash
    git clone https://github.com/albertogeniola/mosquitto-disconnections-plugin
    cp -R mosquitto-disconnections-plugin/src/* mosquitto/plugins/disconnections
    ```
1. Edit mosquitto/plugins/CMakeLists.txt and add the following line at the end of it
    ```
    # ...
    add_subdirectory(disconnections)
    ```
1. You can now run CMake to generate solution files
    ```
    # ...
    cd mosquitto
    cmake .
    ```
