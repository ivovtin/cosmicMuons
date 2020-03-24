# The code for analysis cosmic events
Get the code: <br />
```
git clone https://github.com/ivovtin/cosmicMuons
```
Example for run: <br />
```
analysis_cosmic -x /space/runs/daq025676.nat.bz2    - with KDisplay <br />
analysis_cosmic -x -v 19862 /spool/users/ovtin/sim000010.dat - run simulation with number calibration <br />
```
For run on batch need use next line:
```
bsub batch_dataatc_cosm.sh
bsub batch_mcatc_cosm.sh  - for MC
```

Launch KDisplay for view event with reconstruction: <br />
```
bzcat /space/runs/daq021913.nat.bz2 | KDisplay -r -e3197
```
Launch simulation in KDisplay: <br />
```
/home/ovtin/development/bin/KDisplay < /spool/users/ovtin/sim000004.dat -r -R22996
```
