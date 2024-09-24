make
echo Page Manager Simulation Results > sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'FIFO' -s 10 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'FIFO' -s 25 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'FIFO' -s 50 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'FIFO' -s 100 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'FIFO' -s 200 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'LRU' -s 10 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'LRU' -s 25 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'LRU' -s 50 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'LRU' -s 100 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'LRU' -s 200 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'SC' -s 10 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'SC' -s 25 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'SC' -s 50 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'SC' -s 100 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'SC' -s 200 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'ESC' -s 10 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'ESC' -s 25 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'ESC' -s 25 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'ESC' -s 50 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'ESC' -s 100 >> sim_results.txt
./page_manager_sim.exe -f page_references.txt -m 'ESC' -s 200 >> sim_results.txt
echo END SIMULATION >> sim_results.txt
make clean
