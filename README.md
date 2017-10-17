Course Name: ece275 <br>
Author: <br>
NetID: <br>
Date: <br>
Project Name: project_7

# Description:
The popular game Cards Against Humanity is a horrible game, for horrible people. 
This assignment, fortunately, can be carried out without playing that horrible game (for horrible people). 
One interesting aspect of that game, however, is that it is available under a Creative Commons BY-NC-SA 2.0 license. 
That means you can use, remix, and share the game for free, but you can’t sell it without their permission. 
They kindly asked “Please do not steal our name or we will smash you.” So we haven’t. 
chars Against Formatting is not based on any single game, but any resemblance to games, living or dead, is not just a coincidence.
This assignment requires you to take a card, which is a string with one blank in it, 
and inserts into those blank spaces an appropriate string from a second file. 
A blank is represented as sequence of consecutive underscores. For example, consider the following card: 
<br>
<br>

>
    A Lannister _ _ _ _ _ _ pays his debts. 
>

<br>
<br>

The blank here is a sequence of 6 consecutive underscore characters. Consider the below list of strings:

* Tarth
* spoiler 
* always
* Winter
* never

Your program should look through this list until it finds the first string with the same length as the blank, i.e., a length of 6, 
and output a processed card by replacing the blank with that string. In this example, since always is the first string of length 6, the processed card is:
A Lannister always pays his debts.
If no matching string is found, then no processed card should be written to the output file. 
Each processed card is added to a list of processed card, and the string used in that processed card is removed from the list of strings. 
In the above example, always is removed from the list of strings, resulting in:

* Tarth
* spoiler 
* Winter
* never
