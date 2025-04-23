# CodeSnippets
Proof of knowledge, style, etc. via code examples for use in resume/interviews.

### SDP Parsing:
This C++ Project implemented a open source Session Description Protocol (SDP) parser, sdp-transform, to fit with the the company's parsing needs. SDP is a text format for describing IP video. The original library parsed SDP into Json, however I created an C++ SDP class in SDP.h and SDPEnums.h using the documentation spec RFC 4566 (https://datatracker.ietf.org/doc/html/rfc4566) to save any SDP files we encounter into an easy to use class. The interface I created in SDPParser.cpp makes this conversion possible.

### NmosNodeServer:
This C++ classes implements the NVIDIA/nvnmos library for use in the company's workflow. NMOS is an open source IP video tool that, "enables connection, management, and control of your IP video and audio devices from different manufacturers". NMOS stores SDP files as "Nodes" on an "Node Server" which can be viewed by anyone on the server's network.  This class starts up an NMOS server and creates nodes that allow for connection of video and audio SDP files. These files contain the information needed for anyone to access that IP video and audio.

### NmosScripts:
This folder contains a collection of tools used to test the NmosNodeServer functionality. The batch script starts up an NMOS Registry (not included, but available from open source NMOS SDK) for NMOS Nodes to connect to, then starts up the NmosTestSender which sends an SDP files containing information for IP video and audio to the NmosRegistry. Finally, the NmosController makes a connection between the NmosSender and the NmosReceiver that was started by the NmosNodeServer. I architected this process of testing after struggling with the amount of steps needed to start up all the parts of an NMOS workflow.

### TrainingSetCreator (work in progress):
This Python project is an image processing project which creates frames with interpolated pixels. Digial image interpolation is an image processing technique which uses surrounding known pixel values to estimate new pixel values, or unknown pixel values. In this project, this technique is applied to create new images using different types of pixel interpolation methods (left, right, vertical, and horizontal). The goal of this interpolation is to create new images with less sharp angles in order to train an AI model designed to make animated video smoother.

