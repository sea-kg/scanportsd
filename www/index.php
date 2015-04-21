<html>
	<head>
		<META HTTP-EQUIV="refresh" CONTENT="15">
	</head>
	<body>

<?php

$config = parse_ini_file("/etc/scanportsd/config.ini", true);

$conn = new PDO(
	'mysql:host='.$config['database']['host'].';dbname='.$config['database']['name'].';charset=utf8',
	$config['database']['user'],
	$config['database']['pass']
);

echo "<table cellspacing=1px cellpadding=3px bgcolor=black>
	<tr>
		<td bgcolor=silver>  IP  </td>
		<td bgcolor=silver>  STATUS  </td>
		<td bgcolor=silver>  UPTIME  </td>
		<td bgcolor=silver>  LAST CHECK DATE  </td>
		<td bgcolor=silver>PREVIOUS STATUS (DATE)</td>
	</tr>
";

try {
	$stmt = $conn->prepare('SELECT * FROM servers LIMIT 0,50');
	$stmt->execute();
	while ($row = $stmt->fetch()) {
		echo '<tr>';
		echo '<td bgcolor=white>'.$row['ip'].'</td>';
		
		$stmt_servers_icmp = $conn->prepare('SELECT *, TIMEDIFF(dt_change, dt_create) as uptime FROM servers_icmp WHERE serverid = ? ORDER BY dt_change DESC LIMIT 0,2');
		$stmt_servers_icmp->execute(array(intval($row['id'])));
		
		$status = "?";
		$uptime = "0";
		$date = "?";
		
		$prev_status = "";
		$prev_date = "";
		
		$i = 0;
		while ($row_servers_icmp = $stmt_servers_icmp->fetch()) {
			
			if ($i == 0) {
				$status = $row_servers_icmp['icmp'];
				$date = $row_servers_icmp['dt_change'];
				$uptime = $row_servers_icmp['uptime'];
			} else if ($i == 1) {
				$prev_status = $row_servers_icmp['icmp'];
				$prev_date = $row_servers_icmp['dt_change'];
			}
			
			$i++;
		}
		
		$prev = $prev_status != '' ? $prev_status.'('.$prev_date.')' : 'none';
		
		$color = 'green';
		if ($status == 'DOWN')
			$color = 'red';
		
		echo '
			<td bgcolor='.$color.' align=center>'.$status.'</td>
			<td bgcolor=white align=center>'.$uptime.'</td>
			<td bgcolor=white align=center>'.$date.'</td>
			<td bgcolor=white align=center>'.$prev.'</td>
		</tr>';
	}
} catch(PDOException $e) {
	echo $e->getMessage();
}
echo '</table>';
