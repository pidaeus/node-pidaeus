{
  "targets" : [
    {
      "target_name" : "pidaeus",
      "sources" : [
        "src/pidaeus.cc", 
        "src/gpio.cc"
      ],
      "dependencies" : [
        "deps/libpi/libpi.gyp:pi"
      ]
    }
  ]
}
