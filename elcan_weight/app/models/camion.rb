class Camion < ActiveRecord::Base
  has_many :users
  self.table_name = "trucks"

end
