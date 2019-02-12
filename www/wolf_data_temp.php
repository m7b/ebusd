<?php
date_default_timezone_set('Europe/Berlin');

$con = mysqli_connect("bierfass","wolf","Spacemanwolf", "wolf");

/* check connection */
if (mysqli_connect_errno()) {
    printf("Connect failed: %s\n", mysqli_connect_error());
    exit();
}

//$id   = $_GET["id"];
//$days = $_GET["days"];
$year = $_GET["year"];

$query = "SELECT date, val, DATE_FORMAT(date, '%Y-%m-%d %H') as mydate FROM 0503_aussen_temp";
$query = $query . " WHERE YEAR(date) = " . $year;
$query = $query . " GROUP BY mydate;";


$first = false;


$result = mysqli_query($con, $query);

while($row = mysqli_fetch_array($result))
{
	$datum          = $row['date'];
	$val            = $row['val'];
	$mydate         = $row['mydate'];

	//echo $datum . "\n";


	$datetime = new DateTime($datum);
	$datestr  = $datetime->format('Y-m-d');
	$timestr  = $datetime->format('G');

	if (!$first)
	{
		$nextdatetime = new DateTime($datum);
		$nextdatetime = $nextdatetime->add(new DateInterval('PT1H'));
		$nextdatestr  = $nextdatetime->format('Y-m-d');
		$nexttimestr  = $nextdatetime->format('G');
	}

	//$lastval = $val;
	if ($first)
	{
		while (!((strcmp ($nexttimestr, $timestr) == 0) and (strcmp ($nextdatestr, $datestr) == 0)))
		{
			echo "" . $nextdatestr . "," . $nexttimestr . "," . $lastval . "\n";
			$nextdatetime = $nextdatetime->add(new DateInterval('PT1H'));
			$nextdatestr  = $nextdatetime->format('Y-m-d');
			$nexttimestr  = $nextdatetime->format('G');
		}
	}

	$first = true;

	$nextdatetime = new DateTime($datum);
	$nextdatetime = $nextdatetime->add(new DateInterval('PT1H'));
	$nextdatestr  = $nextdatetime->format('Y-m-d');
	$nexttimestr  = $nextdatetime->format('G');

	$lastval = $val;


	echo $datestr . "," . $timestr . "," . $val . "\n";
}

mysqli_close($con);
?>









