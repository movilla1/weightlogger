# frozen_string_literal: true

ActiveAdmin.register_page 'Stats' do
  menu priority: 2, label: proc { I18n.t('stats') }
  content title: proc { I18n.t('stats') } do
    para I18n.t 'stats_will_show_here'
  end
end
