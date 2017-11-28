google.charts.load('current', {'packages': ['gauge']});
google.charts.setOnLoadCallback(drawChart);

var gauges = {};

function draw(name, value) {

    $container = $('#' + name);
    if (!$container.length) {
        return;
    }

    var label = $container.data('label');
    var data = google.visualization.arrayToDataTable([
        ['Label', 'Value'],
        [label, value]
    ]);

    var options = $container.data();

    if (!gauges[name]) {
        gauges[name] = new google.visualization.Gauge(document.getElementById(name));
    }

    gauges[name].draw(data, options);
}

function drawChart() {
    $.get('/data', function(data) {
        for (var name in data) {
            var value = data[name];
            draw(name, value);
        }
        setTimeout(drawChart, 60000);
    });
}