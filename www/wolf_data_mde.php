<?php
header("Cache-Control: no-cache");

require_once ('setting.php');

date_default_timezone_set('Europe/Berlin');

$con = mysqli_connect($strHostName, $strUserName, $strPassword, $strDbName, $numPort);

/* check connection */
if (mysqli_connect_errno()) {
    printf("Connect failed: %s\n", mysqli_connect_error());
    exit();
}

$id   = $_GET["id"];
$days = $_GET["days"];


switch ($id) {
	case 0:
		$query = "SELECT date, val FROM 0503_kessel_temp";
		break;
	case 1:
		$query = "SELECT date, val FROM 0503_ruecklauf_temp";
		break;
	case 2:
		$query = "SELECT date, val FROM 0503_boiler_temp";
		break;
	case 3:
		$query = "SELECT date, val FROM 0503_aussen_temp";
		break;
}

$days=365;

if ($days > 0) {
	$query = $query . " WHERE DATE_SUB(CURDATE(), INTERVAL " . $days . " DAY) <= date;";
}

$result = mysqli_query($con, $query);

while($row = mysqli_fetch_array($result)) {
  $datum = $row['date'];
  $val   = $row['val'];

  $date = date_create($datum);

  // convert from Unix timestamp to JavaScript time
  $datestr =  $date->getTimestamp() * 1000;

  $data[] = array($datestr,(float) $val);
}

echo json_encode($data);

mysqli_close($con);
?>






























