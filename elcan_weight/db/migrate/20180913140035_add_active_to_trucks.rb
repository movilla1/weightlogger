class AddActiveToTrucks < ActiveRecord::Migration
  def change
    add_column :trucks, :active, :boolean
  end
end
