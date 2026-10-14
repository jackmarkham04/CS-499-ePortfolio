from pymongo import MongoClient
from pymongo.errors import PyMongoError


class AnimalShelter:
    """Handles CRUD operations for the animal shelter database."""

    def __init__(self, username=None, password=None):
        try:
            if username and password:
                self.client = MongoClient(
                    f"mongodb://{username}:{password}@localhost:27017/"
                    f"aac?authSource=admin"
                )
            else:
                self.client = MongoClient("mongodb://localhost:27017/")

            self.database = self.client["aac"]
            self.collection = self.database["animals"]

        except PyMongoError as e:
            print(f"Database connection error: {e}")
            raise

    def create(self, data):
        """Add a new animal record to the database."""

        if not isinstance(data, dict) or not data:
            print("Create error: Data must be a non-empty dictionary.")
            return False

        try:
            self.collection.insert_one(data)
            return True
        except PyMongoError as e:
            print(f"Create error: {e}")
            return False

    def read(self, query):
        """Return animal records that match the supplied query."""

        if not isinstance(query, dict):
            print("Read error: Query must be a dictionary.")
            return []

        try:
            results = self.collection.find(query)
            return list(results)
        except PyMongoError as e:
            print(f"Read error: {e}")
            return []

    def update(self, query, new_values):
        """Update animal records that match the supplied query."""

        if not isinstance(query, dict) or not query:
            print("Update error: A valid query is required.")
            return 0

        if not isinstance(new_values, dict) or not new_values:
            print("Update error: New values must be provided.")
            return 0

        try:
            result = self.collection.update_many(
                query,
                {"$set": new_values}
            )
            return result.modified_count
        except PyMongoError as e:
            print(f"Update error: {e}")
            return 0

    def delete(self, query):
        """Delete animal records that match the supplied query."""

        if not isinstance(query, dict) or not query:
            print("Delete error: A valid query is required.")
            return 0

        try:
            result = self.collection.delete_many(query)
            return result.deleted_count
        except PyMongoError as e:
            print(f"Delete error: {e}")
            return 0