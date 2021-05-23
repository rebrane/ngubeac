# ngubeac
NGU Industries beacon layout optimizer

Uses beam search to try to find optimal map layouts.

```
Application Options:
  -b, --beacons=N         Number of cell types to use: 2=box 3=knight 7=arrows 9=lines 10=donut
  -w, --width=W           Beam width (default 50)
  -g, --generations=G     Generations (default 70)
  -t, --btype=T           Beacon type: 0=speed 1=prod 2=efficiency. Default 1
  -m, --map=M             Map: 0=tutorial 1=flesh 2=tronne
  -f, --file              Filename
  -r, --random            Randomize layout
```

To control which beacons are used, use the -b switch. Increasing the number adds to the available beacons.

Try increasing beam width or generation count for better results. You may want to set generations very high and just ^C after a few hundred.

Doing several runs starting from a random map often gives better results, especially with arrow beacons included.

Beacon type is production by default. Set -t 0 for speed instead of production. Efficiency doesn't work yet.

The 3 first maps are included. Instead you may provide a text file with an alternate map (to start from a certain set of beacons, or from a map that's not fully cleared).
