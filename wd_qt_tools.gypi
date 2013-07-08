{
  'rules': [         
  {
    'rule_name': 'generate_ui',
    'extension': 'ui',
    'outputs': [ '<(RULE_INPUT_DIRNAME)/ui_<(RULE_INPUT_ROOT).h' ],
    'action': [ '<(QT_BIN_PATH)/uic', '<(RULE_INPUT_PATH)', '-o', '<(INTERMEDIATE_DIR)/ui_<(RULE_INPUT_ROOT).h' ],
    'message': 'Generating ui_<(RULE_INPUT_ROOT).h',
  },
  {
    'rule_name': 'generate_moc',
    'extension': 'h',
    'outputs': [ '<(RULE_INPUT_DIRNAME)/moc_<(RULE_INPUT_ROOT).cc' ],
    'action': [ '<(QT_BIN_PATH)/moc', '<(RULE_INPUT_PATH)', '-o', '<(INTERMEDIATE_DIR)/moc_<(RULE_INPUT_ROOT).cc' ],
    'message': 'Generating <(RULE_INPUT_ROOT).cc.',
  } ],
}
