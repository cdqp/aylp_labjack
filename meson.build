# vim: sw=0 noet :

project(
	'aylp_labjack',
	'c',
	default_options: ['c_std=gnu17', 'warning_level=3', 'optimization=3']
	#default_options: ['warning_level=everything', 'optimization=3']
	# this is worth occasionally turning on to check
)

gsl_dep = dependency('gsl')
json_dep = dependency('json-c')
usb_dep = dependency('libusb-1.0')

shared_library('aylp_labjack_dac',
	[
		'aylp_labjack_dac.c', 'labjack_ud.c', 'labjack_u3.c',
		'exodriver/liblabjackusb/labjackusb.c'
	],
	name_prefix: '',
	dependencies: [gsl_dep, json_dep, usb_dep],
	install: true,
	install_dir: '/opt/anyloop',
	include_directories: ['libaylp', 'exodriver/liblabjackusb'],
	# no idea why but --as-needed fails to pull in libusb
	override_options: 'b_lundef=false'
)

