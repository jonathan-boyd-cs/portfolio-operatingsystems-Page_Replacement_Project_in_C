# A Page Manager - Page Replacement Algorithms Exploration

## This project originates from a University of Iowa Operating Systems course.

### The structure of this project deviates significantly from the university submitted rendition.

#### Author: Jonathan Boyd

---

The project exhibits experimentation with page replacement policies. The choice of subjects are FIFO, LRU, Second Chance, and Enhanced Second Chance.<br>
This static implementation simulates the inflow of several thousand page requests, with a desire to observe a difference in efficiency. There are some<br>
108 unique pages in total, whose requests vary from that of reading and writing

---

This project is accompanied with a script and makefile for ease of example.<br>
To run this project...<br>

<ol>
    <li>Move to the respective cloned folder on an Ubuntu machine terminal.</li>
    <li>Type <code>./script.sh</code></li>
    <li>View the results in the <code>sim_results.txt</code> file.</li>
</ol>
<br>
The project can also be manipulated manually. Typing make shall compile the project if a quick solution is desired. Instructions for the command line are
supplied upon initial error, though is also detailed here...
---
<code>./page_manager_sim.exe -f {page_replacement_data_file.txt} -m {replacement_policy} -s {size of memory}</code>
<br> replacement_policy = ( 'FIFO' , 'LRU' , 'SC' , or 'ESC')
---
A new data file may be generated as a the programmer desires, simply following the sytax guide of...<br>
{page_number} {w or r} <- on each line of the text file separated by one space.
<br>
Enjoy... please report discovered errors.
