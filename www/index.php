<?php

$config = parse_ini_file("/etc/scanportsd/config.ini", true);

$conn = new PDO(
	'mysql:host='.$config['database']['host'].';dbname='.$config['database']['name'].';charset=utf8',
	$config['database']['user'],
	$config['database']['pass']
);

echo "<table cellpadding=2px>
	<tr>
		<td>IP</td>
		<td>STATUS</td>
		<td>DATE</td>
	</tr>
";

try {
	$stmt = $conn->prepare('SELECT * FROM servers LIMIT 0,50');
	$stmt->execute();
	while ($row = $stmt->fetch()) {
		echo '<tr>';
		echo '<td>'.$row['ip'].'</td>';
		
		$stmt_servers_icmp = $conn->prepare('SELECT * FROM servers_icmp WHERE serverid = ? ORDER BY dt_change DESC LIMIT 0,2');
		$stmt_servers_icmp->execute(array(intval($row['id'])));
		if ($row_servers_icmp = $stmt_servers_icmp->fetch()) {
			echo '<td align=center>'.$row_servers_icmp['icmp'].'</td>';
			echo '<td align=center>'.$row_servers_icmp['dt_change'].'</td>';
		} else {
			echo '<td align=center>?</td>';
			echo '<td align=center>?</td>';
		}
		echo '</tr>';
	}
} catch(PDOException $e) {
	echo $e->getMessage();
}
echo '</table>';
