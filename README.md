# covid-simulator

This project is a spatially dependent simulator of disease spread over a closed area.
The simulation itself runs in a C++ program, with the user interface presented on a webpage and linked back through AJAX.

Requirements:

-You will need to have Cgicc available for the project to run.

-The Ajax calls expect two files in the cgi-bin: covidsim_file.cgi and covidsim_sim.cgi

-You will need to create a makefile that also moves the text files, web files and C++/.cgi files to appropriate locations.
