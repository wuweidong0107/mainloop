example: log.c example.c mainloop.c 
	gcc  $^ -o $@

clean:
	rm -f example