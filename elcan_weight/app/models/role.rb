# frozen_string_literal: true

class Role < ActiveRecord::Base
  belongs_to :usuario
  enum role: %i[operario admin sysadmin]

end
