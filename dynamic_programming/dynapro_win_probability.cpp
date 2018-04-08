#if 0
/*
The traditional world chess championship is a match of 24 games. The current
champion retains the title in case the match is a tie. Each game ends in a win, loss,
or draw (tie) where wins count as 1, losses as 0, and draws as 1/2. The players take
turns playing white and black. White has an advantage, because he moves first.
The champion plays white in the first game. He has probabilities ww, wd, and wl
of winning, drawing, and losing playing white, and has probabilities bw, bd, and bl
of winning, drawing, and losing playing black.
(a) Write a recurrence for the probability that the champion retains the title.
Assume that there are g games left to play in the match and that the champion
needs to win i games (which may end in a 1/2).
(b) Based on your recurrence, give a dynamic programming to calculate the champion’s
probability of retaining the title

The state transition matrix is (if champ is playing with whites)
WD = diag(ww, wd, wl), similarly BD
After 24 matches, the entire transition matrix would be diag(tw, td, tl) = WD^12 * BD^12
The champ will retain title if tw >= tl

*/

#endif