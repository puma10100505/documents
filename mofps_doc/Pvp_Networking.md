---
theme: "league"
---

---

## Newborn Pvp Engine Networking
### billypu
---

---

## INDEX
---
* Introduction
* Relations of networking objects 
* Important classes of networking
* Communication mechanism between C&S
    - CMD, REP, RPC
    - Channel
    - Session
* How does networking data convert into gameobjects
---

---

## INTRODUCTION
---
* Network protocol：RUDP
* Session: logical object of connected client
* Channel: for networking data transition
    - Receive bytes from client and store into channel
    - Fetch the data from channel and send to client
---

---

## INTRODUCTION
---
* Four kinds of channels for different usage
    - UNRELIABLE_SEQUENCED
    - UNRELIABLE
    - RELIABLE_SEQUENCED
    - STATE_UPDATE 
---

---

## Relations of networking objects
---
<img src="images/NetDriver-ClassDiagram.jpg" alt="" style="border-width: 0"/>
---

---

## Important classes of networking
### - Session & SessionMgr -
---
# TODO
---

--

## Important classes of networking
### - UdpServer -
---
# TODO
---

--

## Important classes of networking
### - Channel & ChannelBuffer -
---
# TODO
---

--

## Important classes of networking
### - NetworkIdentity -
---
# TODO
---

--

## Important classes of networking
### - NetworkManager -
---
# TODO
---

--

## Important classes of networking
### - NetworkConnection -
---
# TODO
---

--

## Important classes of networking
### - NetSocket & NetAddress -
---
# TODO
---

--

## Important classes of networking
### - NetworkBehaviour -
---
# TODO
---

--

## Important classes of networking
### - NetworkConnection -
---
## TODO: Status of Connection
---

---

### Communication mechanism between C&S
---
<img src="images/NetDriver-Commu.jpg" alt="" style="border-width: 0"/>
---