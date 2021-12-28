default: snake

clean:
	rm snake snake.html

snake: snake.cc html
	clang++ -std=c++17 -x objective-c++ -Wno-auto-var-id -framework WebKit -o $@ -I../../../../ $<

.SILENT: html
html: index.html
	echo 'R"(' > snake.html &&\
	cat index.html >> snake.html &&\
	echo '\n)"' >> snake.html
