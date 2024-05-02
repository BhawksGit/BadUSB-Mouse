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
    <li>26 AWG Wire</li>
    <li>Micro USB Solderable Plug or a spliced phone cable</li>
    <li>Soldering Iron & Solder</li>
</ul>

<h2>Installation and Setup</h2>
<p>Refer to the Arduino IDE for uploading the provided <code>20231127_badusb_experimental.ino</code> script to the Arduino Pro Micro. Detailed wiring instructions and setup procedures can be found in the documentation linked below.</p>
<a href="https://github.com/BhawksGit/SmartBadUSB">GitHub Repository</a>

<p>Thank you for exploring the Smart BadUSB project. For more information or to contribute, please visit the GitHub repository linked above.</p>
