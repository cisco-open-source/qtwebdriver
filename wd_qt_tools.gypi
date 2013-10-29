#
# include this file for QT preprocessing - moc, ui, etc...
#
{
  'rules': [         
  {
    'rule_name': 'generate_ui',
    'extension': 'ui',
    'outputs': [ '<(RULE_INPUT_DIRNAME)/ui_<(RULE_INPUT_ROOT).h' ],
    'action': [ 'python', 'do_if_modified.py', '<(QT_BIN_PATH)/uic', '<(RULE_INPUT_PATH)', '<(INTERMEDIATE_DIR)/ui_<(RULE_INPUT_ROOT).h' ],
    'message': 'Generating ui_<(RULE_INPUT_ROOT).h',
  },
  {
    'rule_name': 'generate_moc',
    'extension': 'h',
    'outputs': [ '<(RULE_INPUT_DIRNAME)/moc_<(RULE_INPUT_ROOT).cc' ],
    'action': [ 'python', 'do_if_modified.py','<(QT_BIN_PATH)/moc', '<(RULE_INPUT_PATH)', '<(INTERMEDIATE_DIR)/moc_<(RULE_INPUT_ROOT).cc' ],
    'message': 'Generating <(RULE_INPUT_ROOT).cc.',
  },
  {
    'rule_name': 'generate_rcc',
    'extension': 'qrc',
    'outputs': [ '<(RULE_INPUT_DIRNAME)/rcc_<(RULE_INPUT_ROOT).cc' ],
    'action': [ '<(QT_BIN_PATH)/rcc', '<(RULE_INPUT_PATH)', '-name', '<(RULE_INPUT_ROOT)', '-o', '<(INTERMEDIATE_DIR)/rcc_<(RULE_INPUT_ROOT).cc' ],
    'message': 'Generating <(RULE_INPUT_ROOT).cc.',
  } ],
}
