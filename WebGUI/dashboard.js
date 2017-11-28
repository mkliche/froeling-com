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
        [label, parseInt(value)]
    ]);

    var options = $container.data();

    if (!gauges[name]) {
        gauges[name] = new google.visualization.Gauge(document.getElementById(name));
    }

    gauges[name].draw(data, options);
}

function drawChart() {
    $.get('/froeling', function(data) {
        for (var name in data) {
            var value = data[name];
            draw(name, value);
        }
        setTimeout(drawChart, 60000);
    });
}

$(function() {

    $('form').each(function(index, element) {
        var $form = $(this);
        $.get($(this).attr('action'), function(data) {
            for (var key in data) {
                var value = data[key];
                $form.find('[name="' + key + '"]').not('[type="checkbox"], [type="radio"]').val(value);
                $form.find('[name="' + key + '"]').filter('[type="checkbox"]').prop('checked', !!value);
                $form.find('[name="' + key + '"]').filter('[type="radio"][value="' + value + '"]').prop('checked', true);
            }
        });

        $form.submit(function(event) {
            event.preventDefault();
            var data = {};

            $form.find('input[type="checkbox"]').each(function() {
               data[$(this).attr('name')] = false;
            });
            $form.serializeArray().forEach(function(kvPair) {
                data[kvPair.name] = kvPair.value;
            });

            $.ajax({
                type: "POST",
                url: $form.attr('action'),
                processData: false,
                contentType: 'application/json',
                data: JSON.stringify(data),
                success: function(response) {
                    console.log(response);
                }
            });
        });
    });

    $('#restart').click(function() {
        $.post('/restart');
    });
});