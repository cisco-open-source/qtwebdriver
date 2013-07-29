{
  'includes': [
    'wd.gypi',
    'wd_common.gypi',
  ],

  'targets': [
    {
      'target_name': 'pugixml',
      'type': 'static_library',
      'standalone_static_library': 1,

      'include_dirs': [
        'inc/',
        'src/',
      ],

      'includes': [
        'wd_build_options.gypi',
      ],

      'sources': [
        'src/third_party/pugixml/pugixml.cpp'
      ],
    }
  ],
}
