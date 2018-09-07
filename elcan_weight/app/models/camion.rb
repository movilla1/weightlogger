class Camion < ActiveRecord::Base
  has_many :usuarios
  self.table_name = "camiones"

end
