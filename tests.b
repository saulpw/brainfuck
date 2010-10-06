(Goes to cell 30000 and reports from there with a number)
++++[>++++++<-]>[>+++++>+++++++<<-]>>++++<[[>[[>>+<<-]<]>>>-]>-[>+>+<<-]>]
+++++[>+++++++<<++>-]>.<<.

(Press return and then ^D (^Z on windows)

Two lines should appear:
LA/LA   EOF translates as negative 1
LB/LB   EOF translates as 0
LK/LK   EOF leaves cell unchanged
Anything with O in it means newline is not input as 10 
Anything else is fairly unexpected)

>,>+++++++++,>+++++++++++[<++++++<++++++<+>>>-]<<.>.<<-.>.>.<<.

Should ideally give error message "unmatched bracket" or the like and not give
any output Not essential
+++++[>+++++++>++<<-]>.>.[

(Tests for several obscure problems: Should output an H)
[]++++++++++[>>+>+>++++++[<<+<+++>>>-]<<<<-]
"A*$";?@![#>>+<<]>[>>]<<<<[>++<[-]]>.>.

Should ideally give error message "unmatched ]" or the like and not give
any output
+++++[>+++++++>++<<-]>.>.][

(Tests for several obscure problems: Should output an H)
[]++++++++++[>>+>+>++++++[<<+<+++>>>-]<<<<-]
"A*$";?@![#>>+<<]>[>>]<<<<[>++<[-]]>.>.


My pathological program rot13.b is good for testing the response to deep
brackets; the input "~mlk zyx" should produce the output "~zyx mlk".


For an overall stress test, and also to check whether the output is
monospaced as it ideally should be, I would run numwarp.b.

