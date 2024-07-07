//
// Created by sigsegv on 7/7/24.
//

#include "ZfsSnapshot.h"
#include "ZfsDataset.h"
#include "SimpleExec.h"
#include <iostream>
#include <filesystem>

class ZfsSnapshotImpl {
    friend ZfsSnapshot;
private:
    std::string snapshotname;
    std::string snapshotdir;
    bool cleanupRequired{false};
public:
    ZfsSnapshotImpl(const std::string &dataset, const std::string &snapshot) : snapshotname(dataset) {
        if (snapshot.empty()) {
            throw std::exception();
        }
        snapshotname.append("@");
        snapshotname.append(snapshot);
        if (snapshotname == dataset || snapshotname.empty()) {
            throw std::exception();
        }
        ZfsDataset d{dataset};
        auto mountpoint = d.GetMountpoint();
        if (mountpoint.empty()) {
            std::cerr << "No mountpoint for " << dataset << "\n";
            throw std::exception();
        }
        if (!std::filesystem::exists(mountpoint)) {
            std::cerr << "Mountpoint " << mountpoint << " for " << dataset << " does not exist\n";
            throw std::exception();
        }
        SimpleExec createSnapshot{"/sbin/zfs", {"snapshot", snapshotname}};
        cleanupRequired = true;
        {
            std::filesystem::path path = mountpoint;
            path = path / ".zfs" / "snapshot" / snapshot;
            if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
                std::cerr << "Snapshot directory does not exist or is not a directory: " << path << "\n";
                throw std::exception();
            }
            snapshotdir = path;
        }
        std::cout << "Created snapshot " << snapshotname << " at " << snapshotdir << "\n";

    }
    ~ZfsSnapshotImpl() {
        if (cleanupRequired) {
            cleanupRequired = false;
            std::cout << "Destroying snapshot " << snapshotname << "\n";
            SimpleExec createSnapshot{"/sbin/zfs", {"destroy", snapshotname}};
        }
    }
};

ZfsSnapshot::ZfsSnapshot(const std::string &dataset, const std::string &snapshot) {
    impl = std::make_shared<ZfsSnapshotImpl>(dataset, snapshot);
}

std::string ZfsSnapshot::GetSnapshotName() const {
    return impl ? impl->snapshotname : "";
}

std::string ZfsSnapshot::GetSnapshorDirectory() const {
    return impl ? impl->snapshotdir : "";
}
