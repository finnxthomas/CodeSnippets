'''
~~~~~~

Code Sample License Agreement
Effective Date: 4/15/2025
Cinnafilm, Inc. ("Licensor") grants Finn Thomas ("Licensee") a non-exclusive, 
non-transferable, revocable license to use the provided code samples ("Code") 
under the following terms:
- Permitted Use: Licensee may use the Code solely for personal, non-commercial 
purposes, such as inclusion in a portfolio or demonstration during job 
interviews.
- Restrictions: Licensee may not: (a) use the Code for any commercial purpose; 
(b) distribute, sell, sublicense, or otherwise share the Code with third 
parties; (c) modify the Code for purposes beyond personal demonstration; or 
(d) claim ownership of the Code.
- Ownership: The Code remains the exclusive property of Cinnafilm, Inc.
- Termination: This license may be terminated by Licensor at any time with 
written notice to Licensee, after which Licensee must cease all use of the Code.
No Warranty: The Code is provided "as is," with no warranties of any kind.
By using the Code, Licensee agrees to these terms.

~~~~~
'''

import requests
import json
import socket
import time

class NmosNode:
    def __init__(self, format, id):
        self.format = format
        self.id = id

def get_network_ip():
    hostname = socket.gethostname()
    ip_address = socket.gethostbyname(hostname)
    return ip_address

# Configuration
SENDER_HOST = "http://nmos-sender.local:8086"
RECEIVER_HOST = "http://nmos_reciever.local:8088"

SENDERS = []
RECEIVERS = []

def QueryIDs():
    network_ip = get_network_ip()
    sender_url = f"http://{network_ip}:3211/x-nmos/query/v1.3/senders/"
    receiver_url = f"http://{network_ip}:3211/x-nmos/query/v1.3/receivers/"

    print("Looking for nmos senders...")
    while True:
        senders = requests.get(sender_url).json()

        if len(senders) != 0:
            print("Found nmos sender!")
            break
        else:
            print("Waiting 5 seconds for a receiver to be available...")
            time.sleep(5)
            continue

    for sender in senders:

        if sender["label"] == "NvNmos Audio Sender":
            SENDERS.append(NmosNode("audio", sender["id"]))
        
        elif sender["label"] == "NvNmos Video Sender":
            SENDERS.append(NmosNode("video", sender["id"]))

        else:
            print("Unknown format: ", sender["format"])

    print(SENDERS)

    print("Looking for nmos receivers...")
    while True:
        receivers = requests.get(receiver_url).json()

        if len(receivers) != 0:
            print("Found nmos receiver!")
            break
        else:
            print("Waiting 5 seconds for a receiver to be available...")
            time.sleep(5)
            continue

    for receiver in receivers:
        print(receiver)
        if "audio" in receiver["format"]:
            RECEIVERS.append(NmosNode("audio", receiver["id"]))
        elif "video" in receiver["format"]:
            RECEIVERS.append(NmosNode("video", receiver["id"]))
        else:
            print("Unknown format: ", receiver["format"])

def get_sdp_from_sender(sender_id):
    # Query the sender's transport file (SDP)
    sender_url = f"{SENDER_HOST}/x-nmos/connection/v1.0/single/senders/{sender_id}/transportfile"
    response = requests.get(sender_url)
    response.raise_for_status()
    return response.text


def connect_sender_to_receiver(sender_id, receiver_id, sdp):
    # Prepare the connection activation payload
    payload = {
        ## REDACTED ##
    }

    # Configure the receiver with the sender's SDP
    receiver_url = f"{RECEIVER_HOST}/x-nmos/connection/v1.0/single/receivers/{receiver_id}/staged"
    response = requests.patch(receiver_url, data=json.dumps(payload),
                              headers={"Content-Type": "application/json"})
    response.raise_for_status()

def main():
    try:
        QueryIDs()

        # Get the SDP from the sender
        for sender in SENDERS:
            sdp = get_sdp_from_sender(sender.id)
            print(f"SDP from sender {sender.id}:\n{sdp}")

            # Connect the sender to the correct receiver
            for receiver in RECEIVERS:
                if receiver.format == sender.format:
                    connect_sender_to_receiver(sender.id, receiver.id, sdp)
                    print(f"Successfully connected sender {sender.id} to receiver {receiver.id}")
                    break

    except requests.RequestException as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
