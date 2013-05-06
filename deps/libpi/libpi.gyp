{
  'target_defaults': {
    'default_configuration': 'Release',
    'cflags': [ '-g', '-fvisibility=hidden' ],
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
      },
      'Release': {
        'defines': [ 'NODEBUG' ]
      }
    },
  },

  'targets': [
    {
      'target_name': 'pi',
      'product_prefix': 'lib',
      'type': 'static_library',
      'sources': [
        'include/pi.h',
        'src/common.h',
        'src/cpuinfo.c',
        'src/gpio_mmap.c',
        'src/gpio_event.c',
        'src/timer.c'
      ],
      'include_dirs': [
        'include',
        'src/'
      ],
      'direct_dependent_settings': {
        'include_dirs': [ 
          'include'
        ]
      }
    },

    {
      'target_name': 'example-benchmark',
      'type': 'executable',
      'dependencies': [ 'pi' ],
      'sources': [ 'examples/benchmark/main.c' ]
    },

    {
      'target_name': 'example-led_toggle',
      'type': 'executable',
      'dependencies': [ 'pi' ],
      'sources': [ 'examples/led_toggle/main.c' ]
    },

    {
      'target_name': 'example-stepper',
      'type': 'executable',
      'dependencies': [ 'pi' ],
      'sources': [ 'examples/stepper/main.c' ]
    }

  ]
}
