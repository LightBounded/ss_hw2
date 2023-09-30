# READ-ME

# HW2 - Lexical Analyzer
Authored by Caleb Rivera and Matthew Labrada

## How to run and compile
	Uploading to Eustis:

		(Note: Be sure to be on school network or on the university VPN)
		
		1. Open MobaXTerm

		2. Establish a new SSH session with the following settings:
			Remote Host: eustis3.eecs.ucf.edu
			Specify username: (enter your NID in this field)
			Port: 22

		3. Download and extract the submission into a "ss_hw2" folder, replicating the current submission folder structure.

		4. Use the "cd" command in terminal to go to directory containing the folder you just created.
			(Ex: If you extracted the submission to "C:\Users\{CurrentUser}\Downloads\" then in terminal type: 
			cd Downloads)

		5. Once in that directory, type the following command:
			scp -r ss_hw2 YOUR_NID@eustis3.eecs.ucf.edu:~/
			(Note: Be sure to replace "YOUR_NID" with your actual UCF NID.)
		
	Compiling/Running on Eustis:

		1. In MobaXTerm, type the following command:
			gcc ./ss_hw2/lex.c

		2. Now the program has been compiled. To run the program with a given input file and output file use the following command:
			./a.out "./ss_hw2/input.txt" "./ss_hw2/output.txt"
			(Note: You can replace "input.txt"/"output.txt" with the filename of whatever input/output file you would like to use to run the program. It MUST be located in the "ss_hw2" folder if you use this command though.)

		3. After running these commands, text should be printed onto the screen showing the source program provided by the input file,
            the lexeme table generated from that source program, and its corresponding token list. This same output will be stored in the given
            text file you designated in the previous step.
		
		4. A text file containing this information will be created in the "ss_hw2" directory and will be called "output.txt".