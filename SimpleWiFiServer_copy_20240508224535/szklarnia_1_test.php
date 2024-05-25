<?php
/*************************************************************************************************
 *  Created By: Tauseef Ahmad
 *  Created On: 3 April, 2023
 *  
 *  YouTube Video: https://youtu.be/VEN5kgjEuh8
 *  My Channel: https://www.youtube.com/channel/UCOXYfOHgu-C-UfGyDcu5sYw/
 ***********************************************************************************************/
 
$hostname = "localhost"; 
$username = "root"; 
$password = ""; 
$database = "szklarnia_v3"; 

$conn = mysqli_connect($hostname, $username, $password, $database);

if (!$conn) { 
	die("Connection failed: " . mysqli_connect_error()); 
} 

echo "Database connection is OK<br>"; 

if(isset($_POST["temperature"])&&isset($_POST["humidity"])&&isset($_POST["serial_number"])&&isset($_POST["insolation"]))  {

	$t = $_POST["temperature"];
	$h= $_POST["humidity"];
	$s=$_POST["serial_number"];
	$i=$_POST["insolation"];
	$sql = "INSERT INTO pomiar (Device_id,Temperature,Humidity,Insolation) VALUES (".$s.",".$t.",".$h.",".$i.")"; 

	if (mysqli_query($conn, $sql)) { 
		echo "\nNew record created successfully"; 
	} else { 
		echo "Error: " . $sql . "<br>" . mysqli_error($conn); 
	}
}

?>