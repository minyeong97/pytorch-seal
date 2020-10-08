#include <iostream>

//#include <pybind11/pybind11.h>
//#include <pybind11/stl.h>

#include <iomanip>
#include <memory>
#include <string>

#include "seal/batchencoder.h"
#include "seal/biguint.h"
#include "seal/ciphertext.h"
#include "seal/ckks.h"
#include "seal/context.h"
#include "seal/decryptor.h"
#include "seal/encryptionparams.h"
#include "seal/encryptor.h"
#include "seal/evaluator.h"
#include "seal/intarray.h"
#include "seal/intencoder.h"
#include "seal/keygenerator.h"
#include "seal/memorymanager.h"
#include "seal/modulus.h"
#include "seal/plaintext.h"
#include "seal/publickey.h"
#include "seal/randomgen.h"
#include "seal/randomtostd.h"
#include "seal/relinkeys.h"
#include "seal/secretkey.h"
#include "seal/serialization.h"
#include "seal/valcheck.h"

/*
#include <vector>
class Tensor_enc {
public:
	Tensor_enc()
	{
		array = ArrayXXd(1, 1);
		array(0, 0) = ArrayXXd(3, 3);
		array(0, 0) << 1, 2, 3, 4, 5, 6, 7, 8, 9;
	}

	Tensor_enc(Eigen::ArrayXXd& arr)
	{	
		array = Array<ArrayXXd, 1, 1>(arr)}
	Tensor_enc(Eigen::ArrayXXd arr) : array(arr) { }
	Tensor_enc dot(const Tensor_enc& opp) {
		return Tensor_enc(this->array * opp.array);
	}
	Eigen::ArrayXXd& getArray() { return array; }
	friend std::ostream& operator<<(std::ostream& os, Tensor_enc& tenc);
private:
	Array<ArrayXXd, Dynamic, Dynamic> array;
};


int test(void) {
	return 17;
}

*/

using namespace seal;
using namespace std;

unique_ptr<Encryptor> encryptor;
unique_ptr<Evaluator> evaluator;
unique_ptr<Decryptor> decryptor;
unique_ptr<BatchEncoder> encoder;

namespace seal {

Ciphertext operator+(Ciphertext ct1, Ciphertext ct2) {
	Ciphertext result;
	evaluator->add(ct1, ct2, result);
	return result;
}

Ciphertext operator-(Ciphertext ct1, Ciphertext ct2) {
	Ciphertext result;
	evaluator->sub(ct1, ct2, result);
	return result;
}

Ciphertext operator*(const Ciphertext ct1, const Ciphertext ct2) {
	Ciphertext result;
	evaluator->multiply(ct1, ct2, result);
	return result;
}

Ciphertext operator/(const Ciphertext ct1, const Ciphertext ct2) {
	return ct1;
}

}


#include "eigen3/unsupported/Eigen/CXX11/Tensor"
#include "Eigen/Dense"
#include "eigen3/Eigen/Core"

class Temp {
public:
	Temp() : temp(0) {}
	int temp;
};

Temp operator*(const Temp t1, const Temp t2) {
	return Temp();
}

Temp operator+(const Temp t1, const Temp t2) {
	return Temp();
}

using namespace Eigen;

namespace Eigen {
	
template<>
struct NumTraits<Temp>
{
    typedef Ciphertext Real;
    typedef Ciphertext NonInteger;
    typedef Ciphertext Nested;
  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned = 1,
    RequireInitialization = 1,
    ReadCost = 1,
    AddCost = 3,
    MulCost = 3
  };
};

template<>
struct NumTraits<Ciphertext>
{
    typedef Ciphertext Real;
    typedef Ciphertext NonInteger;
    typedef Ciphertext Nested;
  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned = 1,
    RequireInitialization = 1,
    ReadCost = 1,
    AddCost = 3,
    MulCost = 3
  };
};

}

Tensor<Ciphertext, 2> genTen() {
	Plaintext pt;
	Ciphertext ct;
	Tensor<Ciphertext, 2> m(2, 2);

	auto& d = m.dimensions();

	for(int i = 0; i < d[0]; i++) { 
		for(int j = 0; j < d[1]; j++) {
			encoder->encode(vector<uint64_t>(8192, (i+1)*(j+1)), pt);	
			encryptor->encrypt(pt, ct);
			m(i, j) = ct;
		}
	}
	return m;
}

void dec(Tensor<Ciphertext, 2> t) {
	Tensor<uint64_t, 2> m(2, 2);
	Plaintext pt;
	vector<uint64_t> v;
	auto d = m.dimensions();

	for(int i = 0; i < d[0]; i++) { 
		for(int j = 0; j < d[1]; j++) {
			decryptor->decrypt(t(i, j), pt);
			encoder->decode(pt, v);
			m(i, j) = v[0];
		}
	}

	std::cout << m;

	return;
}


ostream& operator<<(ostream& os, Tensor<Ciphertext, 2> t) {
	std::cout << "__enc__";
	return os;
}

void init() {
    EncryptionParameters parms(scheme_type::BFV);
    size_t poly_modulus_degree = 8192;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));

    parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 20));
    auto context = SEALContext::Create(parms);
    KeyGenerator keygen(context);

    PublicKey public_key = keygen.public_key();
    SecretKey secret_key = keygen.secret_key();
    RelinKeys relin_keys = keygen.relin_keys_local();

    encryptor = unique_ptr<Encryptor>(new Encryptor(context, public_key));
    evaluator = unique_ptr<Evaluator>(new Evaluator(context));
    decryptor = unique_ptr<Decryptor>(new Decryptor(context, secret_key));

    encoder = unique_ptr<BatchEncoder>(new BatchEncoder(context));
}

Tensor<Ciphertext, 3> conv(Tensor<Ciphertext, 3> act, Tensor<Ciphertext, 4> weight, int stride);
Tensor<Ciphertext, 1> fc(Tensor<Ciphertext, 1> act, Tensor<Ciphertext, 2> weight);

Tensor<Ciphertext, 1> flatten(Tensor<Ciphertext, 3> t) {
	auto d = t.dimensions();

	Eigen::array<int, 1> new_dim{{d[0]*d[1]*d[2]}};
	Tensor<Ciphertext, 1> result = t.reshape(new_dim);

	return result;
}

int main(void) {
	init();

	Plaintext pt;
	Ciphertext ct;

	encoder->encode(vector<uint64_t>(8192, 0ULL), pt);	
	encryptor->encrypt(pt, ct);

	Tensor<Ciphertext, 3> act(3, 5, 5);
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 5; j++)
			for(int k = 0; k < 5; k++)
				act(i, j, k) = ct;

	Tensor<Ciphertext, 4> weight(6, 3, 3, 3);
	for(int i = 0; i < 6; i++)
		for(int j = 0; j < 3; j++)
			for(int k = 0; k < 3; k++)
				for(int g = 0; g < 3; g++)
					weight(i, j, k, g) = ct;

	Tensor<Ciphertext, 2> weight_fc(24, 3);
	for(int i = 0; i < 24; i++)
		for(int j = 0; j < 3; j++)
			weight_fc(i, j) = ct;

	Tensor<Ciphertext, 1> res = fc(flatten(conv(act, weight, 2)), weight_fc);
	auto d = res.dimensions();
	std::cout << d[0];
	/*
	Plaintext pt;
	Ciphertext ct;

	encoder->encode(vector<uint64_t>(8192, 14ULL), pt);	
	encryptor->encrypt(pt, ct);

	Plaintext pt1;
	Ciphertext ct1;

	encoder->encode(vector<uint64_t>(8192, 12), pt1);	
	encryptor->encrypt(pt1, ct1);

	Tensor<Ciphertext, 1> m(2);
	m(0) = ct;
	m(1) = ct1;
	Ciphertext ctt = m(0);
	Ciphertext ctt1 = m(1);

	Plaintext pt2;
	vector<uint64_t> v;
	decryptor->decrypt(ctt+ctt1, pt2);
	encoder->decode(pt2, v);
	std::cout << v[0];
	Tensor<int, 3> m(1, 2, 3);
	m.setValues({{{1, 2, 3,},
				  {5, 6, 7}}});

	auto d = m.dimensions();
	std::cout << d.size() << " " << d[0] << " " << d[1] << " " << d[2];
	std::cout << std::endl;

	Eigen::array<int, 2> new_dim{{2, 3}};
	auto res = m.reshape(new_dim);
	auto p = res.dimensions();
	std::cout << p.size() << " " << p[0] << " " << p[1] << " " << p[2];
	*/
	return 0;
}

Ciphertext sum3d(Tensor<Ciphertext, 3> t) {
	auto d = t.dimensions();
	Plaintext pt;
	Ciphertext base;

	encoder->encode(vector<uint64_t>(8192, 0ULL), pt);
	encryptor->encrypt(pt, base);

	for(int i = 0; i < d[0]; i++)
		for(int j = 0; j < d[1]; j++)
			for(int k = 0; k < d[2]; k++)
				evaluator->add_inplace(base, t(i, j, k));

	return base;
}

Ciphertext sum1d(Tensor<Ciphertext, 1> t) {
	auto d = t.dimensions();
	Plaintext pt;
	Ciphertext base;

	encoder->encode(vector<uint64_t>(8192, 0ULL), pt);
	encryptor->encrypt(pt, base);

	for(int i = 0; i < d[0]; i++)
		evaluator->add_inplace(base, t(i));

	return base;
}


Tensor<Ciphertext, 3> conv(Tensor<Ciphertext, 3> act, Tensor<Ciphertext, 4> weight, int stride) {
	auto weight_dimension = weight.dimensions();
	auto act_dimension = act.dimensions();
	size_t weight_output_channel_num = weight_dimension[0];
	size_t act_input_channel_num = act_dimension[0];
	
	// weight's windows should be NxN not MxN
	// calculate the result dimension
	if ((act_dimension[1] - weight_dimension[2]) % stride != 0) {
		std::cout << "Conv::Wrong stride!!!" << std::endl;
	}
	if ((act_dimension[2] - weight_dimension[3]) % stride != 0) {
		std::cout << "Conv::Wrong stride!!!" << std::endl;
	}

	size_t result_height = (act_dimension[1] - weight_dimension[2])/stride + 1;
	size_t result_width = (act_dimension[2] - weight_dimension[3])/stride + 1;
	
	Tensor<Ciphertext, 3> result(weight_output_channel_num, result_height, result_width);

	for(size_t i = 0; i < weight_output_channel_num; i++) {
		for(size_t j = 0; j < result_height; j++) {
			for(size_t k = 0; k < result_width; k++) {
				Eigen::array<int, 3> act_start = {0, stride*j, stride*k};
				Eigen::array<int, 3> act_size = \
					{act_input_channel_num, weight_dimension[2], weight_dimension[3]};
				Eigen::array<int, 4> weight_start = {i, 0, 0, 0};
				Eigen::array<int, 4> weight_size = \
					{1, act_input_channel_num, weight_dimension[2], weight_dimension[3]};
				Tensor<Ciphertext, 3> act_partial = act.slice(act_start, act_size);
				Tensor<Ciphertext, 4> weight_partial = weight.slice(weight_start, weight_size);
				Eigen::array<int, 3> new_dim{{act_input_channel_num, weight_dimension[2], weight_dimension[3]}};
				Tensor<Ciphertext, 3> weight_partial_reshape = weight_partial.reshape(new_dim);
				Tensor<Ciphertext, 3> res = act_partial * weight_partial_reshape;
				result(i, j, k) = sum3d(res);
			}
		}
	}

	return result;
}

Tensor<Ciphertext, 1> fc(Tensor<Ciphertext, 1> act, Tensor<Ciphertext, 2> weight) {
	auto d = weight.dimensions();
	size_t output_channel = d[1]; 
	size_t act_len = d[0];

	Tensor<Ciphertext, 1> result(output_channel);

	for(int i = 0; i < output_channel; i++) {
		Eigen::array<int, 2> weight_start = {0, i};
		Eigen::array<int, 2> weight_size = {act_len, 1};
		Tensor<Ciphertext, 2> weight_partial = weight.slice(weight_start, weight_size);
		Eigen::array<int, 1> new_dim{{act_len}};
		Tensor<Ciphertext, 1> weight_partial_reshape = weight_partial.reshape(new_dim);
		Tensor<Ciphertext, 1> res = act * weight_partial_reshape;
		result(i) = sum1d(res);
	}
	return result;
}
/*
PYBIND11_MODULE(testOctEight, m) {
	pybind11::class_<Tensor_enc>(m, "Tensor_enc")
		.def(pybind11::init<>())
		.def(pybind11::init<Eigen::ArrayXXd>())
		.def("dot", &Tensor_enc::dot);
}
*/
