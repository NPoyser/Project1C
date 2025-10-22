# Project1C
Bitboard

#Author [NPoyser]

#Instructions

Playable squares have '.' it looks like this:

   A B C D E F G H
   -----------------
8 | b   b   b   b
7 |   b   b   b   b
6 | .   .   .   .
5 |   .   .   .   .
4 | .   .   .   .
3 |   *   *   *   *
2 | r   r   r   r
1 |   r   r   r   r
   -----------------

On a checker board there are light and dark squares. The dark squares that are playable is '.' and the light squares that you cant put any checker pieces on is an ' ' space on my board.

The '*' is a highlight to show what pieces can move in what direction. It is currently reds turn so the '*' is showing the direction the red pieces can move in.
You would be asked, Select a piece to move (e.g., A2, or 'quit', 'save', 'load'): a2
After chosing the piece you want to move it will only show the direction that piece can move:

    A B C D E F G H
   -----------------
8 | b   b   b   b
7 |   b   b   b   b
6 | .   .   .   .
5 |   .   .   .   .
4 | .   .   .   .
3 |   *   .   .   .
2 | r   r   r   r
1 |   r   r   r   r
   -----------------

You would then be asked to choose where you want the piece to go.
Select destination (e.g., B3, or 'undo', or 'quit'): b3

Then it will go there:

   A B C D E F G H
   -----------------
8 | b   b   b   b
7 |   b   b   b   b
6 | *   *   *   *
5 |   .   .   .   .
4 | .   .   .   .
3 |   r   .   .   .
2 | .   r   r   r
1 |   r   r   r   r
   -----------------

Afterwards it is blacks turn. You'd move the black piece all the same as the red piece.

A piece can capture a piece, a king can capture a king, a piece can capture a king, and a king can capture a piece.
To capture a piece you have to jump over the opponent piece. For example, instead of the piece at c2 going to b3 where the oppoent piece is, it'd instead jump over it to capture it by moving it to a4.

Visual example:

Select a piece to move (e.g., A2, or 'quit', 'save', 'load'): C2

    A B C D E F G H
   -----------------
8 | b   b   b   b
7 |   b   b   .   b
6 | .   .   .   .
5 |   r   .   .   .
4 | *   .   .   .
3 |   b   *   .   r
2 | .   r   r   .
1 |   r   r   r   r
   -----------------

Select destination (e.g., B3, or 'undo', or 'quit'): A4
Captured a black mortalic piece!

    A B C D E F G H
   -----------------
8 | b   b   b   b
7 |   b   b   *   b
6 | *   *   *   *
5 |   r   .   .   .
4 | r   .   .   .
3 |   .   .   .   r
2 | .   .   r   .
1 |   r   r   r   r
   -----------------
Turn: Black


It would then go to blacks turn.
For a piece to become a king it has to reach the back row. Down for b, up for r. For example:

    A B C D E F G H
   -----------------
8 | b   b   b   b
7 |   .   .   .   b
6 | r   .   b   .
5 |   .   .   .   .
4 | .   .   .   r
3 |   .   r   .   .
2 | b   r   .   .
1 |   *   r   r   r
   -----------------
Turn: Black
Note, '*' highlights the directions you can go.
Select destination (e.g., B3, or 'undo', or 'quit'): B1
Black piece crowned as Immortal King!

    A B C D E F G H
   -----------------
8 | b   b   b   b
7 |   *   .   .   b
6 | r   .   b   .
5 |   .   .   *   *
4 | .   *   *   r
3 |   *   r   .   .
2 | .   r   *   *
1 |   B   r   r   r
   -----------------

To undo, you type undo.
To quit you type quit.
To save you type save and give your save a name.
To load you type load, and then put in the saved name.
I believe it makes more sense to save and load when selecting a piece so I put it there. Same for when choosing a destination, puting undo there is better. So you can't undo in selection, nor save and load when choosing a destination. But you can quit for all of them.

There is an ai. If you want to go against ai just type 'y' at the beginning.

