<!DOCTYPE html>
<html>
<head>
  <link rel="icon" type="image/svg+xml" href="favicon.svg" sizes="any">

  <meta http-equiv="content-type" content="text/html; charset=UTF-8">
  <meta name="robots" content="noindex, nofollow">
  <meta name="googlebot" content="noindex, nofollow">

  <!-- <script type="text/javascript" src="//code.jquery.com/jquery-1.9.1.js"></script> -->
 
  
  <!-- jQuery -->
  <script src="https://code.jquery.com/jquery-latest.js"></script>

  <style type="text/css"></style>

  <title>Highcharts Demo</title>

  <!-- Bootstrap Core CSS -->
  <link href="css/bootstrap.min.css" rel="stylesheet">

  <!-- Custom CSS -->
  <link href="css/stylish-portfolio.css" rel="stylesheet">

  <!-- Custom Fonts -->
  <link href="font-awesome/css/font-awesome.min.css" rel="stylesheet" type="text/css">
  <link href="https://fonts.googleapis.com/css?family=Source+Sans+Pro:300,400,700,300italic,400italic,700italic" rel="stylesheet" type="text/css">


<script type='text/javascript'>//<![CDATA[

$(function () {

    /**
     * This plugin extends Highcharts in two ways:
     * - Use HTML5 canvas instead of SVG for rendering of the heatmap squares. Canvas
     *   outperforms SVG when it comes to thousands of single shapes.
     * - Add a K-D-tree to find the nearest point on mouse move. Since we no longer have SVG shapes
     *   to capture mouseovers, we need another way of detecting hover points for the tooltip.
     */
    (function (H) {
        var Series = H.Series,
            each = H.each;

        /**
         * Create a hidden canvas to draw the graph on. The contents is later copied over
         * to an SVG image element.
         */
        Series.prototype.getContext = function () {
            if (!this.canvas) {
                this.canvas = document.createElement('canvas');
                this.canvas.setAttribute('width', this.chart.chartWidth);
                this.canvas.setAttribute('height', this.chart.chartHeight);
                this.image = this.chart.renderer.image('', 0, 0, this.chart.chartWidth, this.chart.chartHeight).add(this.group);
                this.ctx = this.canvas.getContext('2d');
            }
            return this.ctx;
        };

        /**
         * Draw the canvas image inside an SVG image
         */
        Series.prototype.canvasToSVG = function () {
            this.image.attr({ href: this.canvas.toDataURL('image/png') });
        };

        /**
         * Wrap the drawPoints method to draw the points in canvas instead of the slower SVG,
         * that requires one shape each point.
         */
        H.wrap(H.seriesTypes.heatmap.prototype, 'drawPoints', function () {

            var ctx = this.getContext();

            if (ctx) {

                // draw the columns
                this.points.forEach((point) => {
                    var plotY = point.plotY,
                        shapeArgs,
                        pointAttr;

                    if (plotY !== undefined && !isNaN(plotY) && point.y !== null) {
                        shapeArgs = point.shapeArgs;

                        pointAttr = (point.pointAttr && point.pointAttr['']) || point.series.pointAttribs(point);

                        ctx.fillStyle = pointAttr.fill;
                        ctx.fillRect(shapeArgs.x, shapeArgs.y, shapeArgs.width, shapeArgs.height);
                    }
                });

                this.canvasToSVG();

            } else {
                this.chart.showLoading('Your browser doesn\'t support HTML5 canvas, <br>please use a modern browser');

                // Uncomment this to provide low-level (slow) support in oldIE. It will cause script errors on
                // charts with more than a few thousand points.
                // arguments[0].call(this);
            }
        });
        H.seriesTypes.heatmap.prototype.directTouch = false; // Use k-d-tree
    }(Highcharts));


    var start;
	
    $('#container2015').highcharts({

        data: {
            csv: document.getElementById('csv2015').innerHTML,
            parsed: function () {
                start = +new Date();
            }
        },

        chart: {
            type: 'heatmap',
            margin: [60, 10, 80, 50]
        },

        title: {
            text: 'Highcharts extended heat map',
            align: 'left',
            x: 40
        },

        subtitle: {
            text: 'Temperature variation by day and hour through 2015',
            align: 'left',
            x: 40
        },

        xAxis: {
            type: 'datetime',
            min: Date.UTC(2015, 0, 1),
            max: Date.UTC(2016, 0, 1),
            labels: {
                align: 'left',
                x: 5,
                y: 14,
                format: '{value:%B}' // long month
            },
            showLastLabel: false,
            tickLength: 16
        },

        yAxis: {
            title: {
                text: null
            },
            labels: {
                format: '{value}:00'
            },
            minPadding: 0,
            maxPadding: 0,
            startOnTick: false,
            endOnTick: false,
            tickPositions: [0, 6, 12, 18, 24],
            tickWidth: 1,
            min: 0,
            max: 23,
            reversed: true
        },

        colorAxis: {
            stops: [
                [0, '#004dca'], /* 3060cf */
                [0.5, '#fffbbc'],
                [0.9, '#c4463a'],
                [1, '#640000'] /* c4463a */
            ],
            min: -15,
            max: 35, /*45,*/
            startOnTick: false,
            endOnTick: false,
            labels: {
                format: '{value}℃'
            }
        },

        series: [{
            borderWidth: 0,
            nullColor: '#EFEFEF',
            colsize: 24 * 36e5, // one day
            tooltip: {
                headerFormat: 'Temperature<br/>',
                pointFormat: '{point.x:%e %b, %Y} {point.y}:00: <b>{point.value} ℃</b>'
            },
            turboThreshold: Number.MAX_VALUE // #3404, remove after 4.0.5 release
        }]

    });
	
	
    $('#container2016').highcharts({

        data: {
            csv: document.getElementById('csv2016').innerHTML,
            parsed: function () {
                start = +new Date();
            }
        },

        chart: {
            type: 'heatmap',
            margin: [60, 10, 80, 50]
        },

        title: {
            text: 'Highcharts extended heat map',
            align: 'left',
            x: 40
        },

        subtitle: {
            text: 'Temperature variation by day and hour through 2016',
            align: 'left',
            x: 40
        },

        xAxis: {
            type: 'datetime',
            min: Date.UTC(2016, 0, 1),
            max: Date.UTC(2017, 0, 1),
            labels: {
                align: 'left',
                x: 5,
                y: 14,
                format: '{value:%B}' // long month
            },
            showLastLabel: false,
            tickLength: 16
        },

        yAxis: {
            title: {
                text: null
            },
            labels: {
                format: '{value}:00'
            },
            minPadding: 0,
            maxPadding: 0,
            startOnTick: false,
            endOnTick: false,
            tickPositions: [0, 6, 12, 18, 24],
            tickWidth: 1,
            min: 0,
            max: 23,
            reversed: true
        },

        colorAxis: {
            stops: [
                [0, '#004dca'], /* 3060cf */
                [0.5, '#fffbbc'],
                [0.9, '#c4463a'],
                [1, '#640000'] /* c4463a */
            ],
            min: -15,
            max: 35, /*45,*/
            startOnTick: false,
            endOnTick: false,
            labels: {
                format: '{value}℃'
            }
        },

        series: [{
            borderWidth: 0,
            nullColor: '#EFEFEF',
            colsize: 24 * 36e5, // one day
            tooltip: {
                headerFormat: 'Temperature<br/>',
                pointFormat: '{point.x:%e %b, %Y} {point.y}:00: <b>{point.value} ℃</b>'
            },
            turboThreshold: Number.MAX_VALUE // #3404, remove after 4.0.5 release
        }]

    });
	
	
    $('#container2017').highcharts({

        data: {
            csv: document.getElementById('csv2017').innerHTML,
            parsed: function () {
                start = +new Date();
            }
        },

        chart: {
            type: 'heatmap',
            margin: [60, 10, 80, 50]
        },

        title: {
            text: 'Highcharts extended heat map',
            align: 'left',
            x: 40
        },

        subtitle: {
            text: 'Temperature variation by day and hour through 2017',
            align: 'left',
            x: 40
        },

        xAxis: {
            type: 'datetime',
            min: Date.UTC(2017, 0, 1),
            max: Date.UTC(2018, 0, 1),
            labels: {
                align: 'left',
                x: 5,
                y: 14,
                format: '{value:%B}' // long month
            },
            showLastLabel: false,
            tickLength: 16
        },

        yAxis: {
            title: {
                text: null
            },
            labels: {
                format: '{value}:00'
            },
            minPadding: 0,
            maxPadding: 0,
            startOnTick: false,
            endOnTick: false,
            tickPositions: [0, 6, 12, 18, 24],
            tickWidth: 1,
            min: 0,
            max: 23,
            reversed: true
        },

        colorAxis: {
            stops: [
                [0, '#004dca'], /* 3060cf */
                [0.5, '#fffbbc'],
                [0.9, '#c4463a'],
                [1, '#640000'] /* c4463a */
            ],
            min: -15,
            max: 35, /*45,*/
            startOnTick: false,
            endOnTick: false,
            labels: {
                format: '{value}℃'
            }
        },

        series: [{
            borderWidth: 0,
            nullColor: '#EFEFEF',
            colsize: 24 * 36e5, // one day
            tooltip: {
                headerFormat: 'Temperature<br/>',
                pointFormat: '{point.x:%e %b, %Y} {point.y}:00: <b>{point.value} ℃</b>'
            },
            turboThreshold: Number.MAX_VALUE // #3404, remove after 4.0.5 release
        }]

    });
	
	
    $('#container2018').highcharts({

        data: {
            csv: document.getElementById('csv2018').innerHTML,
            parsed: function () {
                start = +new Date();
            }
        },

        chart: {
            type: 'heatmap',
            margin: [60, 10, 80, 50]
        },

        title: {
            text: 'Highcharts extended heat map',
            align: 'left',
            x: 40
        },

        subtitle: {
            text: 'Temperature variation by day and hour through 2018',
            align: 'left',
            x: 40
        },

        xAxis: {
            type: 'datetime',
            min: Date.UTC(2018, 0, 1),
            max: Date.UTC(2019, 0, 1),
            labels: {
                align: 'left',
                x: 5,
                y: 14,
                format: '{value:%B}' // long month
            },
            showLastLabel: false,
            tickLength: 16
        },

        yAxis: {
            title: {
                text: null
            },
            labels: {
                format: '{value}:00'
            },
            minPadding: 0,
            maxPadding: 0,
            startOnTick: false,
            endOnTick: false,
            tickPositions: [0, 6, 12, 18, 24],
            tickWidth: 1,
            min: 0,
            max: 23,
            reversed: true
        },

        colorAxis: {
            stops: [
                [0, '#004dca'], /* 3060cf */
                [0.5, '#fffbbc'],
                [0.9, '#c4463a'],
                [1, '#640000'] /* c4463a */
            ],
            min: -15,
            max: 35, /*45,*/
            startOnTick: false,
            endOnTick: false,
            labels: {
                format: '{value}℃'
            }
        },

        series: [{
            borderWidth: 0,
            nullColor: '#EFEFEF',
            colsize: 24 * 36e5, // one day
            tooltip: {
                headerFormat: 'Temperature<br/>',
                pointFormat: '{point.x:%e %b, %Y} {point.y}:00: <b>{point.value} ℃</b>'
            },
            turboThreshold: Number.MAX_VALUE // #3404, remove after 4.0.5 release
        }]

    });
	
	
    $('#container2019').highcharts({

        data: {
            csv: document.getElementById('csv2019').innerHTML,
            parsed: function () {
                start = +new Date();
            }
        },

        chart: {
            type: 'heatmap',
            margin: [60, 10, 80, 50]
        },

        title: {
            text: 'Highcharts extended heat map',
            align: 'left',
            x: 40
        },

        subtitle: {
            text: 'Temperature variation by day and hour through 2019',
            align: 'left',
            x: 40
        },

        xAxis: {
            type: 'datetime',
            min: Date.UTC(2019, 0, 1),
            max: Date.UTC(2020, 0, 1),
            labels: {
                align: 'left',
                x: 5,
                y: 14,
                format: '{value:%B}' // long month
            },
            showLastLabel: false,
            tickLength: 16
        },

        yAxis: {
            title: {
                text: null
            },
            labels: {
                format: '{value}:00'
            },
            minPadding: 0,
            maxPadding: 0,
            startOnTick: false,
            endOnTick: false,
            tickPositions: [0, 6, 12, 18, 24],
            tickWidth: 1,
            min: 0,
            max: 23,
            reversed: true
        },

        colorAxis: {
            stops: [
                [0, '#004dca'], /* 3060cf */
                [0.5, '#fffbbc'],
                [0.9, '#c4463a'],
                [1, '#640000'] /* c4463a */
            ],
            min: -15,
            max: 35, /*45,*/
            startOnTick: false,
            endOnTick: false,
            labels: {
                format: '{value}℃'
            }
        },

        series: [{
            borderWidth: 0,
            nullColor: '#EFEFEF',
            colsize: 24 * 36e5, // one day
            tooltip: {
                headerFormat: 'Temperature<br/>',
                pointFormat: '{point.x:%e %b, %Y} {point.y}:00: <b>{point.value} ℃</b>'
            },
            turboThreshold: Number.MAX_VALUE // #3404, remove after 4.0.5 release
        }]

    });
	
	
    $('#container2020').highcharts({

        data: {
            csv: document.getElementById('csv2020').innerHTML,
            parsed: function () {
                start = +new Date();
            }
        },

        chart: {
            type: 'heatmap',
            margin: [60, 10, 80, 50]
        },

        title: {
            text: 'Highcharts extended heat map',
            align: 'left',
            x: 40
        },

        subtitle: {
            text: 'Temperature variation by day and hour through 2020',
            align: 'left',
            x: 40
        },

        xAxis: {
            type: 'datetime',
            min: Date.UTC(2020, 0, 1),
            max: Date.UTC(2021, 0, 1),
            labels: {
                align: 'left',
                x: 5,
                y: 14,
                format: '{value:%B}' // long month
            },
            showLastLabel: false,
            tickLength: 16
        },

        yAxis: {
            title: {
                text: null
            },
            labels: {
                format: '{value}:00'
            },
            minPadding: 0,
            maxPadding: 0,
            startOnTick: false,
            endOnTick: false,
            tickPositions: [0, 6, 12, 18, 24],
            tickWidth: 1,
            min: 0,
            max: 23,
            reversed: true
        },

        colorAxis: {
            stops: [
                [0, '#004dca'], /* 3060cf */
                [0.5, '#fffbbc'],
                [0.9, '#c4463a'],
                [1, '#640000'] /* c4463a */
            ],
            min: -15,
            max: 35, /*45,*/
            startOnTick: false,
            endOnTick: false,
            labels: {
                format: '{value}℃'
            }
        },

        series: [{
            borderWidth: 0,
            nullColor: '#EFEFEF',
            colsize: 24 * 36e5, // one day
            tooltip: {
                headerFormat: 'Temperature<br/>',
                pointFormat: '{point.x:%e %b, %Y} {point.y}:00: <b>{point.value} ℃</b>'
            },
            turboThreshold: Number.MAX_VALUE // #3404, remove after 4.0.5 release
        }]

    });
	
	
    $('#container2021').highcharts({

        data: {
            csv: document.getElementById('csv2021').innerHTML,
            parsed: function () {
                start = +new Date();
            }
        },

        chart: {
            type: 'heatmap',
            margin: [60, 10, 80, 50]
        },

        title: {
            text: 'Highcharts extended heat map',
            align: 'left',
            x: 40
        },

        subtitle: {
            text: 'Temperature variation by day and hour through 2021',
            align: 'left',
            x: 40
        },

        xAxis: {
            type: 'datetime',
            min: Date.UTC(2021, 0, 1),
            max: Date.UTC(2022, 0, 1),
            labels: {
                align: 'left',
                x: 5,
                y: 14,
                format: '{value:%B}' // long month
            },
            showLastLabel: false,
            tickLength: 16
        },

        yAxis: {
            title: {
                text: null
            },
            labels: {
                format: '{value}:00'
            },
            minPadding: 0,
            maxPadding: 0,
            startOnTick: false,
            endOnTick: false,
            tickPositions: [0, 6, 12, 18, 24],
            tickWidth: 1,
            min: 0,
            max: 23,
            reversed: true
        },

        colorAxis: {
            stops: [
                [0, '#004dca'], /* 3060cf */
                [0.5, '#fffbbc'],
                [0.9, '#c4463a'],
                [1, '#640000'] /* c4463a */
            ],
            min: -15,
            max: 35, /*45,*/
            startOnTick: false,
            endOnTick: false,
            labels: {
                format: '{value}℃'
            }
        },

        series: [{
            borderWidth: 0,
            nullColor: '#EFEFEF',
            colsize: 24 * 36e5, // one day
            tooltip: {
                headerFormat: 'Temperature<br/>',
                pointFormat: '{point.x:%e %b, %Y} {point.y}:00: <b>{point.value} ℃</b>'
            },
            turboThreshold: Number.MAX_VALUE // #3404, remove after 4.0.5 release
        }]

    });

    console.log('Rendered in ' + (new Date() - start) + ' ms'); // eslint-disable-line no-console

});
//]]> 
	
</script>
	
	


  
</head>

<body>

<script src="https://code.highcharts.com/stock/highstock.js"></script>
<script src="https://code.highcharts.com/modules/data.js"></script>
<script src="https://code.highcharts.com/modules/heatmap.js"></script>
<script src="https://code.highcharts.com/modules/exporting.js"></script>


<!-- ---------------------------------------------------------------------------- -->


<script type='text/javascript'>//<![CDATA[

	var seriesOptions = [],
		seriesCounter = 0,
		names = ['Kesseltemperatur', 'Rücklauftemperatur', 'Boiler', 'Außentemperatur'];

	/**
	 * Create the chart when all data is loaded
	 * @returns {undefined}
	 */
	function createChart() {
		
		Highcharts.setOptions({
			global: {
				timezone: 'Europe/Berlin',
				useUTC: false
			}
		});

		Highcharts.stockChart('container', {

				rangeSelector: {
					selected: 4,
					allButtonsEnabled: true,
					buttons: [{
							type: 'minute',
							count: 60,
							text: '1h'
					}, {
							type: 'minute',
							count: 180,
							text: '3h'
					}, {
							type: 'minute',
							count: 360,
							text: '6h'
					}, {
							type: 'minute',
							count: 720,
							text: '12h'
					}, {
							type: 'day',
							count: 1,
							text: '1d'
					}, {
							type: 'day',
							count: 2,
							text: '2d'
					},  {
							type: 'day',
							count: 3,
							text: '3d'
					}, {
							type: 'week',
							count: 1,
							text: '1w'
					}]
				},
				title: {
					text: 'Wolf Wärmepumpe'
				},
				subtitle: {
					text: 'Protokoll 0503'
				},
				xAxis: {
					type: 'datetime',
									ordinal: false
				},
				yAxis: {
					title: {
						text: 'Temperatur (°C)'
					}
				},
				legend: {
					enabled: true
				},
				series: seriesOptions
		});
	}
	
	var name_0, name_1, name_2, name_3;
	var val_0, val_1, val_2, val_3;

	$.each(names, function (i, name) {

		$.getJSON('wolf_data_mde.php?id=' + i, function(data){

			seriesOptions[i] = {
					name: name,
					data: data,
					step: true
			};

			// As we're loading the data asynchronously, we don't know what order it will arrive. So
			// we keep a counter and create the chart when all the data is loaded.
			seriesCounter += 1;

			if (seriesCounter == names.length) {
				createChart();
				
				name_0 = seriesOptions[0].name + ": ";
				name_1 = seriesOptions[1].name + ": ";
				name_2 = seriesOptions[2].name + ": ";
				name_3 = seriesOptions[3].name + ": ";
				
				val_0 = seriesOptions[0].data[seriesOptions[0].data.length - 1][1] + " °C";
				val_1 = seriesOptions[1].data[seriesOptions[1].data.length - 1][1] + " °C";
				val_2 = seriesOptions[2].data[seriesOptions[2].data.length - 1][1] + " °C";
				val_3 = seriesOptions[3].data[seriesOptions[3].data.length - 1][1] + " °C";
				
				//console.log(val_0);
				document.getElementById("name_val_0").innerHTML = name_0;
				document.getElementById("demoval_0").innerHTML = val_0;
				document.getElementById("name_val_1").innerHTML = name_1;
				document.getElementById("demoval_1").innerHTML = val_1;
				document.getElementById("name_val_2").innerHTML = name_2;
				document.getElementById("demoval_2").innerHTML = val_2;
				document.getElementById("name_val_3").innerHTML = name_3;
				document.getElementById("demoval_3").innerHTML = val_3;
			}

		});

	});
	
//]]> 
</script>



<!-- ---------------------------------------------------------------------------------- -->












    <!-- Navigation -->
    <a id="menu-toggle" href="#" class="btn btn-dark btn-lg toggle"><i class="fa fa-bars"></i></a>
    <nav id="sidebar-wrapper">
        <ul class="sidebar-nav">
            <a id="menu-close" href="#" class="btn btn-light btn-lg pull-right toggle"><i class="fa fa-times"></i></a>
            <li class="sidebar-brand">
                <a href="#top" onclick=$("#menu-close").click();>Start Bootstrap</a>
            </li>
            <li>
                <a href="#top" onclick=$("#menu-close").click();>Home</a>
            </li>
            <li>
                <a href="#about" onclick=$("#menu-close").click();>About</a>
            </li>
            <li>
                <a href="#chart" onclick=$("#menu-close").click();>BDE/MDE</a>
            </li>
            <li>
                <a href="#temp2021" onclick=$("#menu-close").click();>Heatmap 2021</a>
            </li>
            <li>
                <a href="#temp2020" onclick=$("#menu-close").click();>Heatmap 2020</a>
            </li>
            <li>
                <a href="#temp2019" onclick=$("#menu-close").click();>Heatmap 2019</a>
            </li>
            <li>
                <a href="#temp2018" onclick=$("#menu-close").click();>Heatmap 2018</a>
            </li>
            <li>
                <a href="#temp2017" onclick=$("#menu-close").click();>Heatmap 2017</a>
            </li>
            <li>
                <a href="#temp2016" onclick=$("#menu-close").click();>Heatmap 2016</a>
            </li>
            <li>
                <a href="#temp2015" onclick=$("#menu-close").click();>Heatmap 2015</a>
            </li>
        </ul>
    </nav>

    <!-- Header -->
    <header id="top" class="header">
        <div class="text-vertical-center">
            <h1>Wolf Wärmepumpe</h1>
            <h3>Betriebsdaten &amp; Wetterdaten</h3>
			
			<div class="row">
				<div id="name_val_0" class="col-sm-6" style="text-align:right"></div>
				<div id="demoval_0" class="col-sm-6" style="text-align:left"></div>
			</div>
			
			<div class="row">
				<div id="name_val_1" class="col-sm-6" style="text-align:right"></div>
				<div id="demoval_1" class="col-sm-6" style="text-align:left"></div>
			</div>
			
			<div class="row">
				<div id="name_val_2" class="col-sm-6" style="text-align:right"></div>
				<div id="demoval_2" class="col-sm-6" style="text-align:left"></div>
			</div>
			
			<div class="row">
				<div id="name_val_3" class="col-sm-6" style="text-align:right"></div>
				<div id="demoval_3" class="col-sm-6" style="text-align:left"></div>
			</div>

            <br>
            <a href="#about" class="btn btn-dark btn-lg">Find Out More</a>
        </div>
    </header>
	

    <!-- About -->
    <section id="about" class="about">
        <div class="container">
            <div class="row">
                <div class="col-lg-12 text-center">
                    <h2>Stylish Portfolio is the perfect theme for your next project!</h2>
                    <p class="lead">This theme features some wonderful photography courtesy of <a target="_blank" href="http://join.deathtothestockphoto.com/">Death to the Stock Photo</a>.</p>
                </div>
            </div>
            <!-- /.row -->
        </div>
        <!-- /.container -->
    </section>
	
	
        
  
	
    <!-- chart -->
    <section id="chart" class="chart">
        <div class="container">
            <div class="row">
                <div class="col-lg-12 text-center">
                <div id="container" style="height: 550px; margin: 0 auto"></div></div>
            </div>
            <!-- /.row -->
        </div>
        <!-- /.container -->
    </section>
  
	
  <!-- temp2021 -->
  <section id="temp2021" class="temp2021">
      <div class="container">
          <div class="row">
              <div class="col-lg-12 text-center">
              <div id="container2021" style="height: 320px; margin: 0 auto"></div></div>
          </div>
          <!-- /.row -->
      </div>
      <!-- /.container -->
  </section>
  
	
  <!-- temp2020 -->
  <section id="temp2020" class="temp2020">
      <div class="container">
          <div class="row">
              <div class="col-lg-12 text-center">
              <div id="container2020" style="height: 320px; margin: 0 auto"></div></div>
          </div>
          <!-- /.row -->
      </div>
      <!-- /.container -->
  </section>
  
	
  <!-- temp2019 -->
  <section id="temp2019" class="temp2019">
      <div class="container">
          <div class="row">
              <div class="col-lg-12 text-center">
              <div id="container2019" style="height: 320px; margin: 0 auto"></div></div>
          </div>
          <!-- /.row -->
      </div>
      <!-- /.container -->
  </section>
  
	
  <!-- temp2018 -->
  <section id="temp2018" class="temp2018">
      <div class="container">
          <div class="row">
              <div class="col-lg-12 text-center">
              <div id="container2018" style="height: 320px; margin: 0 auto"></div></div>
          </div>
          <!-- /.row -->
      </div>
      <!-- /.container -->
  </section>
  
	
    <!-- temp2017 -->
    <section id="temp2017" class="temp2017">
        <div class="container">
            <div class="row">
                <div class="col-lg-12 text-center">
                <div id="container2017" style="height: 320px; margin: 0 auto"></div></div>
            </div>
            <!-- /.row -->
        </div>
        <!-- /.container -->
    </section>
	
    <!-- temp2016 -->
    <section id="temp2016" class="temp2016">
        <div class="container">
            <div class="row">
                <div class="col-lg-12 text-center">
                <div id="container2016" style="height: 320px; margin: 0 auto"></div></div>
            </div>
            <!-- /.row -->
        </div>
        <!-- /.container -->
    </section>
	
    <!-- temp2015 -->
    <section id="temp2015" class="temp2015">
        <div class="container">
            <div class="row">
                <div class="col-lg-12 text-center">
                <div id="container2015" style="height: 320px; margin: 0 auto"></div></div>
            </div>
            <!-- /.row -->
        </div>
        <!-- /.container -->
    </section>
	

    <!-- Footer -->
    <footer>
        <div class="container">
            <div class="row">
                <div class="col-lg-10 col-lg-offset-1 text-center">
                    <h4><strong>Start Bootstrap</strong>
                    </h4>
                    <p>3481 Melrose Place
                        <br>Beverly Hills, CA 90210</p>
                    <ul class="list-unstyled">
                        <li><i class="fa fa-phone fa-fw"></i> (123) 456-7890</li>
                        <li><i class="fa fa-envelope-o fa-fw"></i> <a href="mailto:name@example.com">name@example.com</a>
                        </li>
                    </ul>
                    <br>
                    <ul class="list-inline">
                        <li>
                            <a href="#"><i class="fa fa-facebook fa-fw fa-3x"></i></a>
                        </li>
                        <li>
                            <a href="#"><i class="fa fa-twitter fa-fw fa-3x"></i></a>
                        </li>
                        <li>
                            <a href="#"><i class="fa fa-dribbble fa-fw fa-3x"></i></a>
                        </li>
                    </ul>
                    <hr class="small">
                    <p class="text-muted">Copyright &copy; Your Website 2017</p>
                </div>
            </div>
        </div>
        <a id="to-top" href="#top" class="btn btn-dark btn-lg"><i class="fa fa-chevron-up fa-fw fa-1x"></i></a>
    </footer>


	
	
	
	
	
	
	
	
	
	
	
	<!-- Custom Theme JavaScript -->
    <script>
    // Closes the sidebar menu
    $("#menu-close").click(function(e) {
        e.preventDefault();
        $("#sidebar-wrapper").toggleClass("active");
    });
    // Opens the sidebar menu
    $("#menu-toggle").click(function(e) {
        e.preventDefault();
        $("#sidebar-wrapper").toggleClass("active");
    });
    // Scrolls to the selected menu item on the page
    $(function() {
        $('a[href*=#]:not([href=#],[data-toggle],[data-target],[data-slide])').click(function() {
            if (location.pathname.replace(/^\//, '') == this.pathname.replace(/^\//, '') || location.hostname == this.hostname) {
                var target = $(this.hash);
                target = target.length ? target : $('[name=' + this.hash.slice(1) + ']');
                if (target.length) {
                    $('html,body').animate({
                        scrollTop: target.offset().top
                    }, 1000);
                    return false;
                }
            }
        });
    });
    //#to-top button appears after scrolling
    var fixed = false;
    $(document).scroll(function() {
        if ($(this).scrollTop() > 250) {
            if (!fixed) {
                fixed = true;
                // $('#to-top').css({position:'fixed', display:'block'});
                $('#to-top').show("slow", function() {
                    $('#to-top').css({
                        position: 'fixed',
                        display: 'block'
                    });
                });
            }
        } else {
            if (fixed) {
                fixed = false;
                $('#to-top').hide("slow", function() {
                    $('#to-top').css({
                        display: 'none'
                    });
                });
            }
        }
    });
    // Disable Google Maps scrolling
    // See http://stackoverflow.com/a/25904582/1607849
    // Disable scroll zooming and bind back the click event
    var onMapMouseleaveHandler = function(event) {
        var that = $(this);
        that.on('click', onMapClickHandler);
        that.off('mouseleave', onMapMouseleaveHandler);
        that.find('iframe').css("pointer-events", "none");
    }
    var onMapClickHandler = function(event) {
            var that = $(this);
            // Disable the click handler until the user leaves the map area
            that.off('click', onMapClickHandler);
            // Enable scrolling zoom
            that.find('iframe').css("pointer-events", "auto");
            // Handle the mouse leave event
            that.on('mouseleave', onMapMouseleaveHandler);
        }
        // Enable map zooming with mouse scroll when the user clicks the map
    $('.map').on('click', onMapClickHandler);
    </script>





<!-- Source: http://vikjavev.no/ver/highcharts-demos/heatmap.csv.php?year=2013 -->

<pre id="csv2021" style="display: none">Date,Time,Temperature
<?php include 'https://wolf.mjbsoft.de/wolf_data_temp.php?year=2021'; //need php-opt: allow_url_include = 1 ?>
</pre>

<pre id="csv2020" style="display: none">Date,Time,Temperature
<?php include 'https://wolf.mjbsoft.de/wolf_data_temp.php?year=2020'; //need php-opt: allow_url_include = 1 ?>
</pre>

<pre id="csv2019" style="display: none">Date,Time,Temperature
<?php include 'https://wolf.mjbsoft.de/wolf_data_temp.php?year=2019'; //need php-opt: allow_url_include = 1 ?>
</pre>

<pre id="csv2018" style="display: none">Date,Time,Temperature
<?php include 'https://wolf.mjbsoft.de/wolf_data_temp.php?year=2018'; //need php-opt: allow_url_include = 1 ?>
</pre>

<pre id="csv2017" style="display: none">Date,Time,Temperature
<?php include 'https://wolf.mjbsoft.de/wolf_data_temp.php?year=2017'; //need php-opt: allow_url_include = 1 ?>
</pre>

<pre id="csv2016" style="display: none">Date,Time,Temperature
<?php include 'https://wolf.mjbsoft.de/wolf_data_temp.php?year=2016'; //need php-opt: allow_url_include = 1 ?>
</pre>

<pre id="csv2015" style="display: none">Date,Time,Temperature
<?php include 'https://wolf.mjbsoft.de/wolf_data_temp.php?year=2015'; //need php-opt: allow_url_include = 1 ?>
</pre>
  
</body>

</html>

