#!/usr/bin/env python3

import os
import cgi

print("Status: 200")
print("Content-Type: text/html")
print("")
print("<html><body>")
print("<h2>Hello from CGI</h2>")
form = cgi.FieldStorage()
if form.getvalue("name"):
    print(f"<p>Hi, {form.getvalue('name')}!</p>")
else:
    print("<p>No name provided</p>")

print("</body></html>")
