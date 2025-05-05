<?php
header("Content-Type: text/html");
echo "<html><body><h1>PHP CGI</h1>";
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
  echo "<p>POST received. Email: " . htmlspecialchars($_POST['email']) . "</p>";
} else {
  echo "<p>METHOD: " . $_SERVER['REQUEST_METHOD'] . "</p>";
  echo "<p>PATH_INFO: " . $_SERVER['PATH_INFO'] . "</p>";
}
echo "</body></html>";
?>
