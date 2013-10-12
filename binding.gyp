{
  "targets" : [
    {
      "target_name" : "pidaeus",
      "sources" : [
        "src/pidaeus.cc", 
        "src/gpio.cc",
        "src/gpio_async.cc"
      ],
      "dependencies" : [
        "deps/libpi/libpi.gyp:pi"
      ],
      "include_dirs" : [
        "<!(node -p -e \"require('path').relative('.', require('path').dirname(require.resolve('nan')))\")"
      ]
    }
  ]
}
