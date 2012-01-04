Plots in snostream
==================
Global
------
### 1D ###
* global rate (x axis time, y axis - rate)
* per trigger type - trigger rate
* trigger counts - (x axis trigger type, y axis counts)
* reconstructed mtc triggers - time versus counts
* nhit distribution - nhit versus counts
* avg nhit - avg nhit versus time

### 2D ###
* crate versus board versus event count
* crate versus board versus event rate
* crate versus board versus cmos rate
* crate versus board versus cgt 24-bit error
* crate versus board versus hardware error
* crate versus board versus cgt 16-bit error
* crate versus board versus cmos es 16-bit error
* crate versus board versus lgiselect error
* crate versus board versus missed count
* crate versus board versus anomalous qhl value count
* crate versus board versus anomalous qhs value count
* crate versus board versus anomalous qlx value count
* crate versus board versus anomalous tac value count

Per-crate
---------
### 1D ###
* events per channel
* event rate per channel (events per second)
* counts versus time (x axis - minutes, y axis - total count)

### 2D ###
* board versus channel versus events
* board versus channel versus event rate
* board versus channel versus cmos rate
* board versus channel versus cgt 24-bit error
* board versus channel versus hardware error
* board versus channel versus cgt 16-bit error
* board versus channel versus cmos es 16-bit error
* board versus channel versus lgiselect error
* board versus channel versus missed count
* board versus channel versus anomalous qhl value count
* board versus channel versus anomalous qhs value count
* board versus channel versus anomalous qlx value count
* board versus channel versus anomalous tac value count

Questionable
------------
### 1D ###
* t-win
* nhit min
* duration
* burst size
* breakdown_trigger
* development_trigger
* supernova_trigger

### 2D ###
* board versus channel versus cmos thresholds (per crate)
* board versus channel versus cmos threshold zeroes (per crate)
* board versus channel versus cmos voltage balances (per crate)
