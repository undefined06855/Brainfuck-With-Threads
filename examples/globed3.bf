[
  This will try its hardest to send a get request to 127.0.0.1:3000
  obviously uses syscalls so linux only!
]

call socket(2 1 0) to get socket fd
+++++++++++++++++++++++++++++++++++++++++> syscall 41
+++> three params
+ > ++> two (int)
+ > +> one (int)
+ > > zero (int)

<<<<<<<<!

for connect it's more complex
connect(socket_fd (address of)sockaddr_in sizeof(sockaddr_in))
cell    45  46  47  48  49  50  51  52  53  54  55  56  57  58  59
value   42   3   1  fd   2   ?   ?   ?   ?   ?   ?   ?   ?   1  16

cell 1 now has socket fd
copy to cell 48
[>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-]

get address of cell 10
>>>>>>>>>&
copy to cell 50 to 58
10 to 50 [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-] >
11 to 51 [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-] >
12 to 52 [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-] >
13 to 53 [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-] >
14 to 54 [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-] >
15 to 55 [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-] >
16 to 56 [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-] >
17 to 57 [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-] >
<<<<<<<<

currently at cell 10 we now need to fill with sockaddr_in data
this is correct according to chatgpt for 127 0 0 1 port 3000

cell   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26
value   2   0  11 184 127   0   0   1   0   0   0   0   0   0   0   0   0
  ++
> 
> +++++++++++
> ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
> +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>
>
> +
>
>
>
>
>
>
>
>
>

set cell 45 to 42
>>>>>>>>>>>>>>>>>>> ++++++++++++++++++++++++++++++++++++++++++

set cell 46 to 3
> +++

set cell 47 to 1
> +

cell 48 is file descriptor copied earlier
>

set cell 49 to 2
> ++

cell 50 to 57 are address of cell 10 copied earlier
>>>>>>>>

set cell 58 to 1
> +

set cell 59 to 16
> ++++++++++++++++

and syscall
<<<<<<<<<<<<<<!







now we need to write to it using write
cell   45  46  47  48  49  50  51  52  53  54  55  56  57  58  59
value   1   3   1  fd   2   ?   ?   ?   ?   ?   ?   ?   ?   1  35

pointer and fd are in the same place thankfully so no copying needed

set cell 59 to 35 from 16
>>>>>>>>>>>>>>+++++++++++++++++++

cell 58 can be left identical (type int)
<

cell 50 to 57 can be left identical (ptr to data)
<<<<<<<<

cell 49 can be left identical (type long)
<

cell 48 can be left identical (fd)
<

cell 47 can be left identical (type int)
<

cell 46 can be left identical (param count 3)
<

set cell 45 to 1
< [-] +


now we need to fill cell 10 with the string data before doing the syscall
but clear every cell as we go past
<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]<[-]

now fill cell 10 to 42 with the string data
GET / HTTP/1(dot)1\r\nHost: 127(dot)0(dot)0(dot)1\r\n

GET
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/
>++++++++++++++++++++++++++++++++
>+++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++
HTTP/1 1
>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>+++++++++++++++++++++++++++++++++++++++++++++++
>+++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++
>+++++++++++++++++++++++++++++++++++++++++++++++++
\r\n
>+++++++++++++
>++++++++++
Host:
>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++
127 0 0 1
>+++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++++++
>+++++++++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++++
>++++++++++++++++++++++++++++++++++++++++++++++
>+++++++++++++++++++++++++++++++++++++++++++++++++
\r\n
>+++++++++++++
>++++++++++

we're now at cell 42 at the end of the string
go to cell 45 and syscall
>>>!


before reading copy our fd to somewhere where the buffer wont overrun it
cell 4 should do so it can be used to close later
but clear cell 4 first idk what's in it
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<[-]>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
then go to our fd and copy
>>>
copying is so hard we have to move to cell 3 and 4
[-<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<+<+>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>]
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
then move cell 3 back
[->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<]
somewhere in all of this there's an off by one error so we need to subtract one from the fd
but its fine it works this is the worst bf ive ever written
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>-<<<

next we read
we can read just to the same buffer as the address to reduce having to copy around addresses
cell   45  46  47  48  49  50  51  52  53  54  55  56  57  58  59
value   0   3   1  fd   2   ?   ?   ?   ?   ?   ?   ?   ?   1  255

set cell 45 to 0
[-]

we dont mind overrunning our "buffer" so we can set the buffer size to 255

set cell 59 from 35 to 255 by decrementing 36
>>>>>>>>>>>>>>
------------------------------------

go to cell 45 and syscall
<<<<<<<<<<<<<<!

now memory from cell 10 to 265 should be filled with our data
go to cell 10 and print it off
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.>.
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

now we're at cell 1
setup call to close
cell   1  2  3  4 
value  3  1  1  fd
who knows what's in these cells so just clear them before setting anything up

 [-] +++
>[-] +
>[-] +
cell four has our fd dont clear that

then syscall to close
<<!

and we're done
